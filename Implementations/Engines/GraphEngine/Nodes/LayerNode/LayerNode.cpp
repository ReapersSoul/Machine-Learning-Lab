#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>

class LayerNode : public NodeInterface {
	std::vector<double> x;
	std::vector<double> b;
	std::vector<std::vector<double>> w;
	std::vector<double> z;
	std::vector<double> ForwardGradient;
	int inputs=1, outputs=1;
	double LearningRate=0.1;
	ActivationInterface* Activation;
	int max_threads;

	bool CapDerivative = false;
	double DerivativeCeil = 1;
	double DerivativeFloor = 0;

	bool GPU = false;

	cl::Program program;
	cl::Context context;
	cl::CommandQueue queue;

	double RandRange(double min, double max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}

	void RandomizeWeights() {
		for (int i = 0; i < outputs; i++)
		{
			for (int j = 0; j < inputs; j++)
			{
				w[i][j] = RandRange(-1,1);
			}
		}
		for (int i = 0; i < outputs; i++)
		{
			b[i] = RandRange(-1,1);
		}
	}

	std::vector<double> Flatten(std::vector<std::vector<double>>& v) {
		std::vector<double> result;
		for (int i = 0; i < v.size(); i++)
		{
			for (int j = 0; j < v[i].size(); j++)
			{
				result.push_back(v[i][j]);
			}
		}
		return result;
	}

	std::vector<std::vector<double>> Reshape(std::vector<double>& v, int rows, int cols) {
		std::vector<std::vector<double>> result;
		for (int i = 0; i < rows; i++)
		{
			std::vector<double> row;
			for (int j = 0; j < cols; j++)
			{
				row.push_back(v[i * cols + j]);
			}
			result.push_back(row);
		}
		return result;
	}

	std::vector<double> ForwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b) {
		//if network is not sized propperly resize
		if (x.size() != inputs) {
			inputs = x.size();
			w.resize(outputs);
			for (int i = 0; i < outputs; i++)
			{
				w[i].resize(inputs);
			}
			b.resize(outputs);
			z.resize(outputs);
			RandomizeWeights();
		}

		for (int i = 0; i < outputs; i++)
		{
			z[i] = 0;
			for (int j = 0; j < inputs; j++)
			{
				z[i] += x[j] * w[i][j];
			}
			z[i] += b[i];
		}
		std::vector<double> a(z.size());
		for (int i = 0; i < z.size(); i++)
		{
			a[i] = Activation->Activate(z[i]);
		}
		return a;
	}

	std::vector<double> BackwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b, std::vector<double>& ForwardGradient) {
		std::vector<double> dA_dX(inputs);
		for (int i = 0; i < inputs; i++)
		{
			dA_dX[i] = 0;
			for (int j = 0; j < outputs; j++)
			{
				dA_dX[i] += ForwardGradient[j] * w[j][i] * Activation->ActivateDerivative(z[j]);
			}
		}
		for (int i = 0; i < outputs; i++)
		{
			b[i] -= ForwardGradient[i] * Activation->ActivateDerivative(z[i]) * LearningRate;
			for (int j = 0; j < inputs; j++)
			{
				w[i][j] -= ForwardGradient[i] * Activation->ActivateDerivative(z[i]) * x[j] * LearningRate;
			}
		}
		return dA_dX;
	}

	std::vector<double> GPUForwardLayer(std::vector<double>& x, std::vector<std::vector<double>>& w, std::vector<double>& z, std::vector<double>& b) {
		cl::Kernel Forward_Kernel(program, "Forward");

		cl::Buffer buffer_x(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());
		cl::Buffer buffer_w(context, CL_MEM_READ_WRITE, sizeof(double) * w.size() * w[0].size());
		cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, sizeof(double) * b.size());
		cl::Buffer buffer_z(context, CL_MEM_READ_WRITE, sizeof(double) * z.size());

		// Enqueue buffer writes
		queue.enqueueWriteBuffer(buffer_x, CL_TRUE, 0, sizeof(double) * x.size(), x.data());
		queue.enqueueWriteBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), Flatten(w).data());
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

		cl::Kernel Backward_Kernel(program, "Backward");

		cl::Buffer buffer_x(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());
		cl::Buffer buffer_w(context, CL_MEM_READ_WRITE, sizeof(double) * w.size() * w[0].size());
		cl::Buffer buffer_b(context, CL_MEM_READ_WRITE, sizeof(double) * b.size());
		cl::Buffer buffer_ZPrime(context, CL_MEM_READ_WRITE, sizeof(double) * ZPrime.size());
		cl::Buffer buffer_XPrime(context, CL_MEM_READ_WRITE, sizeof(double) * x.size());

		// Enqueue buffer writes
		queue.enqueueWriteBuffer(buffer_x, CL_TRUE, 0, sizeof(double) * x.size(), x.data());
		queue.enqueueWriteBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), Flatten(w).data());
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
		Backward_Kernel.setArg(11, static_cast<int>(CapDerivative));
		Backward_Kernel.setArg(12, static_cast<double>(DerivativeCeil));
		Backward_Kernel.setArg(13, static_cast<double>(DerivativeFloor));

		// Enqueue the kernel
		queue.enqueueNDRangeKernel(Backward_Kernel, cl::NullRange, cl::NDRange(max_threads), cl::NullRange);

		// Read w, b, and xprime
		std::vector<double> new_w(w.size() * w[0].size());
		queue.enqueueReadBuffer(buffer_w, CL_TRUE, 0, sizeof(double) * w.size() * w[0].size(), new_w.data());
		w=Reshape(new_w, w.size(), w[0].size());
		queue.enqueueReadBuffer(buffer_b, CL_TRUE, 0, sizeof(double) * b.size(), b.data());

		std::vector<double> da_dx(x.size());
		queue.enqueueReadBuffer(buffer_XPrime, CL_TRUE, 0, sizeof(double) * x.size(), da_dx.data());

		for (size_t i = 0; i < da_dx.size(); i++)
			da_dx[i] = std::min(da_dx[i], DerivativeCeil);
		return da_dx;
	}

public:
	LayerNode() {
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
		file.open("FullyConnectedLayerKernel.cl", std::ios::in);
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
			return;
		}
		SetTypeID("LayerNode");
		max_threads = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	}

	void Init() override {
		MakeInput(0, "Input", "double", nlohmann::json::array());
		MakeInput(1, "Output Size", "double", nlohmann::json::array());
		MakeOutput(0, "Output", "double", nlohmann::json::array());

		Activation = AE->GetAvailableActivations()[0];

		MakeAttribute(0, new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Cap Derivatives?", &CapDerivative);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Ceil", &DerivativeCeil);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Floor", &DerivativeFloor);
			}));

		MakeAttribute(1, new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if(ImGui::Button("Randomize Weights"))
				RandomizeWeights();
			}));

		MakeAttribute(2, new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Use GPU?", &GPU);
			}));

		MakeAttribute(3, new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Outputs", &outputs);
			}));

		MakeAttribute(4,new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Learning Rate", &LearningRate);
			}));

		MakeAttribute(5,new Attribute([this]() {
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
		MakeAttribute(6,new Attribute([this]() {
			ImGui::PushItemWidth(100);
			//imgui table
			if (ImGui::BeginTable("MyTable", 3)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Column 2", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Column 3", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableNextRow();
				// Data for Column 1
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Inputs")) {
					for (int i = 0; i < Description.size(); i++) {
						if (Description[i].find("Input") != Description[i].end()) {
							ImGui::Text(Description[i]["Input"]["Data"].dump().c_str());
						}
					}
				}

				// Data for Column 2
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Weights")) {
					for (int i = 0; i < w.size(); i++) {
						for (int j = 0; j < w[i].size(); j++) {
							ImGui::Text(("w" + std::to_string(i) + ", " + std::to_string(j) + ": " + std::to_string(w[i][j])).c_str());
						}
					}
				}

				// Data for Column 3
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Outputs")) {
					for (int i = 0; i < Description.size(); i++) {
						if (Description[i].find("Output") != Description[i].end()) {
							//right align text
							ImGui::Indent(10);
							ImGui::Text(Description[i]["Output"]["Data"].dump().c_str());
						}
					}
				}

				ImGui::EndTable();
			}

			}));
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			bool resize = false;
			if (inputs != GetInputDataByIndex(0)[0]["Data"].size()) {
				inputs = GetInputDataByIndex(0)[0]["Data"].size();
				resize = true;
			}
			if (!GetInputDataByIndex(1)[0]["Data"].is_null()) {
				if (outputs != GetInputDataByIndex(1)[0]["Data"].size()) {
					outputs = GetInputDataByIndex(1)[0]["Data"].size();
					resize = true;
				}
			}
			if (resize) {
				x.resize(inputs);
				b.resize(outputs);
				w.resize(outputs);
				z.resize(outputs);
				ForwardGradient.resize(outputs);
				for (int i = 0; i < outputs; i++)
				{
					w[i].resize(inputs);
				}
				RandomizeWeights();
			}
			for (int i = 0; i < x.size(); i++)
			{
				x[i] = GetInputDataByIndex(0)[0]["Data"][i].get<double>();
			}
			nlohmann::json data = nlohmann::json::object();
			if (GPU) {
				data["Data"] = GPUForwardLayer(x, w, z, b);
			}
			else {
				data["Data"] = ForwardLayer(x, w, z, b);
			}
			data["Type"]= "Vector";
			GetOutputDataByIndex(0) = data;
		}
		else {
			ForwardGradient.clear();
			GetInputDataByIndex(0)=nlohmann::json::object();
			for (int i = 0; i < outputs; i++)
			{
				ForwardGradient.push_back(GetOutputDataByIndex(0)[0]["Data"][i].get<double>()*LearningRate);
			}
			if(GPU){
				GetInputDataByIndex(0)["Data"] = GPUBackwardLayer(x, w, z, b, ForwardGradient);
			}
			else {
				GetInputDataByIndex(0)["Data"] = BackwardLayer(x, w, z, b, ForwardGradient);
			}
			GetInputDataByIndex(0)["Type"] = "Vector";
		}
	}

	//void DrawNodeTitle();

	void Update() override {
	}

	NodeInterface* GetInstance() {
		LayerNode* node = new LayerNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		//cap gradients
		data["CapDerivative"] = CapDerivative;

		//learning rate
		data["LearningRate"] = LearningRate;

		//inputs
		data["Inputs"] = inputs;

		//outputs
		data["Outputs"] = outputs;

		//weights
		data["Weights"] = w;

		//bias
		data["Bias"] = b;

		//activation
		data["Activation"] = Activation->GetName();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		//cap gradients
		if (!data["CapDerivative"].is_null()) {
			CapDerivative = data["CapDerivative"].get<bool>();
		}

		//learning rate
		if (!data["LearningRate"].is_null()) {
			LearningRate = data["LearningRate"].get<double>();
		}

		//inputs
		if (!data["Inputs"].is_null()) {
			inputs = data["Inputs"].get<int>();
		}

		//outputs
		if (!data["Outputs"].is_null()) {
			outputs = data["Outputs"].get<int>();
		}

		//resize
		x.resize(inputs);
		b.resize(outputs);
		w.resize(outputs);
		z.resize(outputs);
		ForwardGradient.resize(outputs);
		for (int i = 0; i < outputs; i++)
		{
			w[i].resize(inputs);
		}

		//weights
		if (!data["Weights"].is_null()) {
			w = data["Weights"].get<std::vector<std::vector<double>>>();
		}

		//bias
		if (!data["Bias"].is_null()) {
			b = data["Bias"].get<std::vector<double>>();
		}

		//activation
		if (!data["Activation"].is_null()) {
			//loop through available activations
			for (int i = 0; i < AE->GetAvailableActivations().size(); i++) {
				//if the name matches
				if (AE->GetAvailableActivations()[i]->GetName() == data["Activation"].get<std::string>()) {
					//set the activation
					Activation = AE->GetAvailableActivations()[i];
				}
			}
		}

		return;
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "LayerNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NodeInterface* GetInstance() {
		return new LayerNode();
	}
}