#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <string>

class RecurrentLayerNode : public NodeInterface {
	std::vector<double> x;
	std::vector<double> b;
	std::vector<std::vector<double>> w;
	std::vector<double> z;
	std::vector<double> a;
	std::vector<double> ForwardGradient;
	int inputs = 1, outputs = 1;
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
		for (int i = 0; i < outputs; i++)
		{
			for (int j = 0; j < inputs+outputs; j++)
			{
				w[i][j] = RandRange(-.01,.01);
			}
		}
		for (int i = 0; i < outputs; i++)
		{
			b[i] = RandRange(-.01,.01);
		}
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


		std::vector<double> da_dx(x.size());
		queue.enqueueReadBuffer(buffer_XPrime, CL_TRUE, 0, sizeof(double) * x.size(), da_dx.data());

		return da_dx;
	}

public:
	RecurrentLayerNode() {
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
		TypeID = "RecurrentLayerNode";
		max_threads = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	}

	void Init() override {
		MakeInput(0, "Input", "double", nlohmann::json::array());
		MakeOutput(0, "Output", "double", nlohmann::json::array());

		Activation = AE->GetAvailableActivations()[0];

		MakeAttribute(1, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputInt("Outputs", &outputs);
			}));

		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Learning Rate", &LearningRate);
			}));

		MakeAttribute(3, new AttributeInterface([this]() {
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
		MakeAttribute(4, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			//imgui table
			if (ImGui::BeginTable("MyTable", 3)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Column 2", ImGuiTableColumnFlags_WidthFixed, 100.0f);
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
				for (int i = 0; i < w.size(); i++) {
					for (int j = 0; j < w[i].size(); j++) {
						ImGui::Text(("w" + std::to_string(i) + ", " + std::to_string(j) + ": " + std::to_string(w[i][j])).c_str());
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
		if (DirectionForward) {
			if (GetInputDataByIndex(0).is_array()) {
				if (GetInputDataByIndex(0).size() != inputs) {
					inputs = GetInputDataByIndex(0).size();
					x.resize(inputs+outputs);
					b.resize(outputs);
					w.resize(outputs);
					z.resize(outputs);
					a.resize(outputs, 0);
					ForwardGradient.resize(outputs);
					for (int i = 0; i < outputs; i++)
					{
						w[i].resize(inputs+outputs);
					}
					RandomizeWeights();

				}
				for (int i = 0; i < inputs; i++)
				{
					x[i] = GetInputDataByIndex(0)[i].get<double>();
				}
				for (int i = inputs; i < inputs+outputs; i++)
				{
					x[i] = a[i - inputs];
				}
			}
			else {
				x[0] = GetInputDataByIndex(0).get<double>();
			}


			a= GPUForwardLayer(x, w, z, b);

			GetOutputDataByIndex(0) = a;
		}
		else {
			ForwardGradient.clear();
			if (GetOutputDataByIndex(0).is_array()) {
				int j = GetOutputDataByIndex(0).size();
				for (int i = 0; i < GetOutputDataByIndex(0).size(); i++)
				{
					ForwardGradient.push_back(GetOutputDataByIndex(0)[i].get<double>());
				}
			}
			else {
				ForwardGradient[0] = GetOutputDataByIndex(0).get<double>();
			}

			GetInputDataByIndex(0) = GPUBackwardLayer(x, w, z, b, ForwardGradient);
		}
	}

	//void DrawNodeTitle();

	void Update() override {

		if (inputs < 1) {
			inputs = 1;
		}
		if (outputs < 1) {
			outputs = 1;
		}

		if ((x.size() != inputs+outputs) || (z.size() != outputs)) {
			//resize x and all other vectors
			x.resize(inputs+outputs);
			b.resize(outputs);
			w.resize(outputs);
			z.resize(outputs);
			ForwardGradient.resize(outputs);
			for (int i = 0; i < outputs; i++)
			{
				w[i].resize(inputs+outputs);
			}
			RandomizeWeights();
		}
	}

	NodeInterface* GetInstance() {
		RecurrentLayerNode* node = new RecurrentLayerNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		return;
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new RecurrentLayerNode();
	}
}