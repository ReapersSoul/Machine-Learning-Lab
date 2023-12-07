#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <string>
#include <glm/glm.hpp>

#include <typeinfo>

#include <CL/opencl.hpp>

//TODO: Fix forward and backward passes
class RecurrentNetworkNode : public NodeInterface {
	std::vector<double>x;
	std::vector<std::vector<double>> b;
	std::vector<std::vector<std::vector<double>>> w;
	std::vector<std::vector<double>> z;
	std::vector<std::vector<double>> a;
	char tmp_layers[256] = "2,1";

	int inputSize = 1;
	std::vector<int> layers = { 2,1 };
	double LearningRate = 0.1;
	ActivationInterface* Activation;
	int max_threads;

	cl::Program program;
	cl::Context context;
	cl::CommandQueue queue;

	double RandRange(double min, double max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}

	void RandomizeWeights() {
		for (int i = 0; i < w.size(); i++)
		{
			for (int j = 0; j < w[i].size(); j++)
			{
				for (int k = 0; k < w[i][j].size(); k++)
				{
					w[i][j][k] = RandRange(-.01,.01);
				}
			}
		}
		for (int i = 0; i < b.size(); i++)
		{
			for (int j = 0; j < b[i].size(); j++)
			{
				b[i][j] = RandRange(-.01,.01);
			}
		}
	}

	void ResizeLayer(int inputSize, int outputSize, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b) {
		w.resize(outputSize);
		z.resize(outputSize, 0);
		b.resize(outputSize, 0);
		for (int i = 0; i < outputSize; i++)
		{
			w[i].resize((size_t)inputSize+outputSize, 0);
		}
	}

	void ResizeNetwork(int InputSize, std::vector<int> LayerSizes) {
		x.resize((size_t)InputSize+LayerSizes[0]);

		w.resize(LayerSizes.size());
		z.resize(LayerSizes.size());
		b.resize(LayerSizes.size());
		a.resize(LayerSizes.size());

		ResizeLayer(InputSize+LayerSizes[0], LayerSizes[0], w[0], z[0], b[0]);
		a[0].resize(LayerSizes[0]+LayerSizes[1], 0);
		for (int i = 1; i < LayerSizes.size(); i++)
		{
			ResizeLayer(LayerSizes[(size_t)i - 1] + LayerSizes[i], LayerSizes[i], w[i], z[i], b[i]);
			if (i < LayerSizes.size() - 1) {
				a[i].resize(LayerSizes[i] + LayerSizes[i + 1], 0);
			}
			else {
				a[i].resize(LayerSizes[i], 0);
			}
		}
	}

	std::vector<double> ForwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b) {
		int outputSize = w.size();
		int inputSize = w[0].size();
		std::vector<double> a(outputSize, 0);
		for (int i = 0; i < outputSize; i++)
		{
			z[i] = 0;
			for (int j = 0; j < inputSize; j++)
			{
				z[i] += w[i][j] * x[j];
			}
			z[i] += b[i];
			a[i] = Activation->Activate(z[i]);
		}
		return a;
	}

	std::vector<double> BackwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b, std::vector<double>& ForwardGradient) {
		int outputSize = w.size();
		int inputSize = w[0].size();
		std::vector<double> da_dx(inputSize, 0);
		for (int i = 0; i < outputSize; i++)
		{
			for (int j = 0; j < inputSize; j++)
			{
				da_dx[j] += w[i][j] * Activation->ActivateDerivative(z[i]) * ForwardGradient[i];
				w[i][j] += x[j] * Activation->ActivateDerivative(z[i]) * ForwardGradient[i] * LearningRate;
			}
			b[i] += Activation->ActivateDerivative(z[i]) * ForwardGradient[i] * LearningRate;
		}
		return da_dx;
	}

	std::vector<double> GPUForwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b) {
		cl::Kernel Forward_Kernel(program, "Forward");

		cl::Buffer buffer_x(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());
		cl::Buffer buffer_w(context, CL_MEM_READ_WRITE, sizeof(double) * w.size() * w[0].size());
		cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, sizeof(double) * b.size());
		cl::Buffer buffer_z(context, CL_MEM_READ_WRITE, sizeof(double) * z.size());

		// Enqueue buffer writes
		queue.enqueueWriteBuffer(buffer_x, CL_TRUE, 0, sizeof(double) * x.size(), x.data());
		queue.enqueueWriteBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), w.data());
		queue.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, sizeof(double) * b.size(), b.data());

		// Set kernel arguments
		Forward_Kernel.setArg(0, buffer_x);
		Forward_Kernel.setArg(1, static_cast<int>(x.size())); // Use static_cast for safer conversion
		Forward_Kernel.setArg(2, buffer_w);
		Forward_Kernel.setArg(3, static_cast<int>(w.size()));
		Forward_Kernel.setArg(4, static_cast<int>(w[0].size()));
		Forward_Kernel.setArg(5, buffer_b);
		Forward_Kernel.setArg(6, static_cast<int>(b.size()));
		Forward_Kernel.setArg(7, buffer_z);
		Forward_Kernel.setArg(8, static_cast<int>(z.size()));

		// Enqueue the kernel
		queue.enqueueNDRangeKernel(Forward_Kernel, cl::NullRange, cl::NDRange(z.size()), cl::NullRange);

		// Read the result from buffer_z
		queue.enqueueReadBuffer(buffer_z, CL_TRUE, 0, sizeof(double) * z.size(), z.data());

		// Activate z
		std::vector<double> a(z.size());
		for (size_t i = 0; i < z.size(); i++) {
			a[i] = Activation->Activate(z[i]);
		}
		return a;
	}

	std::vector<double> GPUBackwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b, std::vector<double>& ForwardGradient) {
		std::vector<double> ZPrime(z.size());
		for (size_t i = 0; i < z.size(); i++) {
			ZPrime[i] = Activation->ActivateDerivative(z[i]) * ForwardGradient[i] * LearningRate;
		}
		std::vector<std::vector<double>> old_w = w;
		std::vector<double> old_b = b;

		cl::Kernel Backward_Kernel(program, "Backward");

		cl::Buffer buffer_x(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());
		cl::Buffer buffer_w(context, CL_MEM_READ_WRITE, sizeof(double) * w.size() * w[0].size());
		cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, sizeof(double) * b.size());
		cl::Buffer buffer_ZPrime(context, CL_MEM_READ_WRITE, sizeof(double) * ZPrime.size());
		cl::Buffer buffer_XPrime(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());

		// Enqueue buffer writes
		queue.enqueueWriteBuffer(buffer_x, CL_TRUE, 0, sizeof(double) * x.size(), x.data());
		queue.enqueueWriteBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), w.data());
		queue.enqueueWriteBuffer(buffer_b, CL_TRUE, 0, sizeof(double) * b.size(), b.data());
		queue.enqueueWriteBuffer(buffer_ZPrime, CL_TRUE, 0, sizeof(double) * ZPrime.size(), ZPrime.data());

		// Set kernel arguments
		Backward_Kernel.setArg(0, buffer_x);
		Backward_Kernel.setArg(1, static_cast<int>(x.size())); // Use static_cast for safer conversion
		Backward_Kernel.setArg(2, buffer_w);
		Backward_Kernel.setArg(3, static_cast<int>(w.size()));
		Backward_Kernel.setArg(4, static_cast<int>(w[0].size()));
		Backward_Kernel.setArg(5, buffer_b);
		Backward_Kernel.setArg(6, static_cast<int>(b.size()));
		Backward_Kernel.setArg(7, buffer_ZPrime);
		Backward_Kernel.setArg(8, static_cast<int>(ZPrime.size()));
		Backward_Kernel.setArg(9, buffer_XPrime);
		Backward_Kernel.setArg(10, static_cast<int>(x.size()));

		// Enqueue the kernel
		queue.enqueueNDRangeKernel(Backward_Kernel, cl::NullRange, cl::NDRange(max_threads), cl::NullRange);

		// Read w, b, and xprime
		queue.enqueueReadBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), w.data());
		queue.enqueueReadBuffer(buffer_b, CL_TRUE, 0, sizeof(double) * b.size(), b.data());

		//print new weights
		for (int j = 0; j < w.size(); j++) {
			for (int k = 0; k < w[j].size(); k++) {
				printf("w[%d][%d]: %f -> %f Old==New:%s\n", j, k, old_w[j][k], w[j][k], old_w[j][k] == w[j][k] ? "true" : "false");
			}
		}

		// print new biases
		for (int j = 0; j < b.size(); j++) {
			printf("b[%d]: %f -> %f Old==New:%s\n", j, old_b[j], b[j], old_b[j] == b[j] ? "true" : "false");
		}

		std::vector<double> da_dx(x.size());
		queue.enqueueReadBuffer(buffer_XPrime, CL_TRUE, 0, sizeof(double) * x.size(), da_dx.data());

		return da_dx;
	}



	std::vector<std::string> Split(std::string str, char delimiter) {
		std::vector<std::string> internal;
		std::stringstream ss(str); // Turn the string into a stream.
		std::string tok;

		while (getline(ss, tok, delimiter)) {
			internal.push_back(tok);
		}

		return internal;
	}
public:
	RecurrentNetworkNode() {
		//setup gpu
		cl::Platform platform = cl::Platform::getDefault();
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		cl::Device device = devices[0];
		context = cl::Context(device);
		queue = cl::CommandQueue(context, device);
		cl::Program::Sources sources;
		//convolution kernel
		std::string kernel_code;
		//load kernel code from file
		std::fstream file;
		file.open("RecurrentLayerKernel.cl", std::ios::in);
		std::string line;
		while (std::getline(file, line)) {
			kernel_code += line + "\n";
		}
		file.close();

		sources.push_back({ kernel_code.c_str(),kernel_code.length() });
		program = cl::Program(context, sources);
		if (program.build({ device }) != CL_SUCCESS) {

			std::fstream file;
			file.open("kernel_error.txt", std::ios::out);
			file << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
			file.close();
			printf(" Error building: %s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str());
		}
		TypeID = "RecurrentNetworkNode";
		max_threads = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	}

	void Init() override {
		ResizeNetwork(inputSize, layers);
		RandomizeWeights();
		MakeInput(0, "Input", "double", nlohmann::json::array());
		MakeOutput(0, "Output", "double", nlohmann::json::array());

		Activation = AE->GetAvailableActivations()[0];
		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginCombo("Activation", Activation->GetName().c_str())) {
				for (int i = 0; i < AE->GetAvailableActivations().size(); i++)
				{
					bool selected = false;
					ImGui::Selectable(AE->GetAvailableActivations()[i]->GetName().c_str(), &selected);
					if (selected) {
						Activation = AE->GetAvailableActivations()[i];
					}
				}
				ImGui::EndCombo();
			}
			}));

		MakeAttribute(1, new AttributeInterface([this]() {
			//attribute for layer sizes
			ImGui::PushItemWidth(100);
			if (ImGui::InputText("Layers", tmp_layers, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
				std::vector<std::string> layersS = Split(tmp_layers, ',');
				layers.resize(layersS.size());
				for (int i = 0; i < layersS.size(); i++)
				{
					layers[i] = std::stoi(layersS[i]);
				}
				ResizeNetwork(inputSize, layers);
				RandomizeWeights();
			}
			}));
		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Learning Rate", &LearningRate);
			}));

		MakeAttribute(4, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			//imgui table
			if (ImGui::BeginTable("MyTable", 3)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Column 2", ImGuiTableColumnFlags_WidthFixed, 200.0f);
				ImGui::TableSetupColumn("Column 3", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableNextRow();
				// Data for Column 1
				ImGui::TableNextColumn();
				for (int i = 0; i < Description.size(); i++) {
					if (Description[i].find("Input") != Description[i].end()) {
						ImGui::Text(Description[i]["Input"]["Data"].dump().c_str());
					}
				}

				// Data for Column 2
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Weights")) {
					for (int i = 0; i < w.size(); i++) {
						for (int j = 0; j < w[i].size(); j++) {
							for (int k = 0; k < w[i][j].size(); k++) {
								//use display 10 decimal places
								char tmp[256];
								sprintf_s(tmp, "%.20f", w[i][j][k]);
								ImGui::PushItemWidth(200);
								ImGui::Text(("w" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(k) + ": " + std::string(tmp)).c_str());
								ImGui::PopItemWidth();
							}
						}
					}
				}

				// Data for Column 3
				ImGui::TableNextColumn();
				for (int i = 0; i < Description.size(); i++) {
					if (Description[i].find("Output") != Description[i].end()) {
						//right align text
						ImGui::Indent(10);
						ImGui::Text(Description[i]["Output"]["Data"].dump().c_str());
					}
				}

				ImGui::EndTable();
			}
			}));

	}

	void Process(bool DirectionForward) override {
		printf("Processing Node %d\n", UID);
		ResizeNetwork(inputSize, layers);
		if (DirectionForward) {
			//get input
			if (GetInputDataByIndex(0).is_array()) {
				if (inputSize != GetInputDataByIndex(0).size()) {
					inputSize = GetInputDataByIndex(0).size();
					ResizeNetwork(inputSize, layers);
					RandomizeWeights();
				}

				for (int i = 0; i < inputSize; i++) {
					x[i] = GetInputDataByIndex(0)[i].get<double>();
				}
				for (int i = layers[0]; i < layers[0]*2; i++)
				{
					x[i] = a[0][i - (size_t)layers[0]];
				}
			}
			else {
				x[0] = GetInputDataByIndex(0).get<double>();
			}

			a[0] = GPUForwardLayer(x, w[0], z[0], b[0]);
			for (int i = layers[0]; i < layers[0]*2; i++)
			{
				a[0][i] = a[1][i-(size_t)layers[0]];
			}

			for (int l = 1; l < layers.size(); l++) {
				a[l] = GPUForwardLayer(a[l - (size_t)1], w[l], z[l], b[l]);
				if (l < layers.size() - 1) {
					for (int i = layers[l]; i < layers[l] * 2; i++)
					{
						a[l][i] = a[l + (size_t)1][i - (size_t)layers[l]];
					}
				}
			}

			// push last activation to output
			GetOutputDataByIndex(0).clear();
			for (int i = 0; i < a.back().size(); i++) {
				GetOutputDataByIndex(0).push_back(a.back()[i]);
			}
		}
		else {
			// Get ForwardGradient from output
			std::vector<double> ForwardGradient;
			if (GetOutputDataByIndex(0).is_array()) {
				for (int i = 0; i < GetOutputDataByIndex(0).size(); i++) {
					ForwardGradient.push_back(GetOutputDataByIndex(0)[i].get<double>());
				}
			}
			else {
				ForwardGradient.push_back(GetOutputDataByIndex(0).get<double>());
			}

			for (int l = layers.size() - 1; l > 0; l--) {
				ForwardGradient = GPUBackwardLayer(a[l - (size_t)1], w[l], z[l], b[l], ForwardGradient);
			}
			ForwardGradient = GPUBackwardLayer(x, w[0], z[0], b[0], ForwardGradient);

			// push ForwardGradient to input
			GetInputDataByIndex(0).clear();
			for (int i = 0; i < ForwardGradient.size(); i++) {
				GetInputDataByIndex(0).push_back(ForwardGradient[i]);
			}
		}
	}

	void Update() override {

	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		return;
	}

	NodeInterface* GetInstance() {
		return new RecurrentNetworkNode();
	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "RecurrentNetworkNode";
	}

	__declspec(dllexport) NodeInterface* GetInstance() {
		return new RecurrentNetworkNode();
	}
}
