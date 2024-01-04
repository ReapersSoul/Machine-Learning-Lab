#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

//opencl
#include <CL/opencl.hpp>

//stb image write
#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined(_MSC_VER)
	//  Microsoft 
	#define __STDC_LIB_EXT1__
#endif
#include <stb_image_write.h>
//stb image read
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct CustomException : public std::exception {
	std::string message;
	CustomException(std::string message) : message(message) {}

	const char* what() const throw () {
		return message.c_str();
	}
};

class ConvolutionNode : public NS_Node::NodeInterface {
	std::vector<double> K;
	std::vector<double> Y;
	std::vector<double> X;
	std::vector<double> Z;
	double LearningRate = 0.01;
	char* KImageData;
	GLuint KTexture;
	char* YImageData;
	GLuint YTexture;
	NS_Activation::ActivationInterface* Activation;

	glm::ivec3 X_size{0, 0,0};
	glm::ivec3 K_size{3, 3,3};
	glm::ivec3 stride{1, 1,1};
	glm::ivec3 padding{2, 2,2};
	glm::ivec3 Y_size{0, 0,0};
	glm::ivec3 threads{32, 32,32};

	std::string filepath;
	std::string SavePath;

	double RandRange(double min, double max) {
		return min + (max - min) * (double)rand() / RAND_MAX;
	}

	void RandomizeK() {
		for (int i = 0; i < K.size(); i++)
		{
			K[i] = RandRange(-1, 1);
		}
	}

	template <typename T>
	T map(T Input, T InputMin, T InputMax, T YMin, T YMax) {
		return (T)(((double)Input - (double)InputMin) * ((double)YMax - (double)YMin) / ((double)InputMax - (double)InputMin) + (double)YMin);
	}

	void TensorToTexture(GLuint& texture, std::vector<double> tensor, glm::ivec3 size) {
		//check if texture exists
		if (texture != 0) {
			glDeleteTextures(1, &texture);
		}

		std::vector<unsigned char> ImageData;
		ImageData.resize(tensor.size());
		for (int i = 0; i < tensor.size(); i++)
		{
			ImageData[i] = (unsigned char)map(tensor[i], -1.0, 1.0, 0.0, 255.0);
		}

		//Grey-scale
		// Create a OpenGL texture identifier
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		//if (size.z == 1) {
		//	//fill the gba with r
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
		//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		
		//}
		//if (size.z == 2) {
		//	//fill the ba with rg
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
		//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//}


		// Upload pixels into texture
		if (size.z == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ImageData.data());
		}

		if (size.z == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData.data());
		}

		if (size.z == 43) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData.data());
		}
	}

	glm::ivec2 GetOutputSize(glm::ivec2 input_size, glm::ivec2 K_size, glm::ivec2 stride, glm::ivec2 padding) {
		//Y Size = ((Input Size - K Size + 2 * Padding) / Stride) + 1
		glm::ivec2 YSize = glm::ivec2(0, 0);
		YSize.x = (input_size.x - K_size.x + 2 * padding.x) / stride.x + 1;
		YSize.y = (input_size.y - K_size.y + 2 * padding.y) / stride.y + 1;
		return YSize;
	}

	const char* getErrorString(cl_int error)
	{
		switch (error) {
			// run-time and JIT compiler errors
		case 0: return "CL_SUCCESS";
		case -1: return "CL_DEVICE_NOT_FOUND";
		case -2: return "CL_DEVICE_NOT_AVAILABLE";
		case -3: return "CL_COMPILER_NOT_AVAILABLE";
		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5: return "CL_OUT_OF_RESOURCES";
		case -6: return "CL_OUT_OF_HOST_MEMORY";
		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8: return "CL_MEM_COPY_OVERLAP";
		case -9: return "CL_IMAGE_FORMAT_MISMATCH";
		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11: return "CL_BUILD_PROGRAM_FAILURE";
		case -12: return "CL_MAP_FAILURE";
		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15: return "CL_COMPILE_PROGRAM_FAILURE";
		case -16: return "CL_LINKER_NOT_AVAILABLE";
		case -17: return "CL_LINK_PROGRAM_FAILURE";
		case -18: return "CL_DEVICE_PARTITION_FAILED";
		case -19: return "CL_K_ARG_INFO_NOT_AVAILABLE";

			// compile-time errors
		case -30: return "CL_INVALID_VALUE";
		case -31: return "CL_INVALID_DEVICE_TYPE";
		case -32: return "CL_INVALID_PLATFORM";
		case -33: return "CL_INVALID_DEVICE";
		case -34: return "CL_INVALID_CONTEXT";
		case -35: return "CL_INVALID_QUEUE_PROPERTIES";
		case -36: return "CL_INVALID_COMMAND_QUEUE";
		case -37: return "CL_INVALID_HOST_PTR";
		case -38: return "CL_INVALID_MEM_OBJECT";
		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40: return "CL_INVALID_IMAGE_SIZE";
		case -41: return "CL_INVALID_SAMPLER";
		case -42: return "CL_INVALID_BINARY";
		case -43: return "CL_INVALID_BUILD_OPTIONS";
		case -44: return "CL_INVALID_PROGRAM";
		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46: return "CL_INVALID_KERNEL_NAME";
		case -47: return "CL_INVALID_KERNEL_DEFINITION";
		case -48: return "CL_INVALID_KERNEL";
		case -49: return "CL_INVALID_ARG_INDEX";
		case -50: return "CL_INVALID_ARG_VALUE";
		case -51: return "CL_INVALID_ARG_SIZE";
		case -52: return "CL_INVALID_KERNEL_ARGS";
		case -53: return "CL_INVALID_WORK_DIMENSION";
		case -54: return "CL_INVALID_WORK_GROUP_SIZE";
		case -55: return "CL_INVALID_WORK_ITEM_SIZE";
		case -56: return "CL_INVALID_GLOBAL_OFFSET";
		case -57: return "CL_INVALID_EVENT_WAIT_LIST";
		case -58: return "CL_INVALID_EVENT";
		case -59: return "CL_INVALID_OPERATION";
		case -60: return "CL_INVALID_GL_OBJECT";
		case -61: return "CL_INVALID_BUFFER_SIZE";
		case -62: return "CL_INVALID_MIP_LEVEL";
		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64: return "CL_INVALID_PROPERTY";
		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66: return "CL_INVALID_COMPILER_OPTIONS";
		case -67: return "CL_INVALID_LINKER_OPTIONS";
		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

			// extension errors
		case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
		case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
		case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
		case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
		case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
		case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
		default: return "Unknown OpenCL error";
		}
	}

	void printError(cl_int error, const char* function, int line, const char* file, bool should_throw = false) {
		if (error != CL_SUCCESS) {
			printf("Error: %s\nWhile: %s\nLine: %d\nFile: %s\n", getErrorString(error), function, line, file);
			if (should_throw) {
				//throw custom exception
				throw CustomException("Error: " + std::string(getErrorString(error)) + "\nWhile: " + std::string(function) + "\nLine: " + std::to_string(line) + "\nFile: " + std::string(file));
			}
		}
	}

	//very broken
	std::vector<double> GPUConvolution3D(std::vector<double> x, std::vector<double> k, glm::ivec3 xSize, glm::ivec3 kSize, glm::ivec3 stride, glm::ivec3 padding, glm::ivec3 Y_size) {
		//make buffers
		cl::Buffer input_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, xSize.x * xSize.y * xSize.z * sizeof(double));
		cl::Buffer K_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, kSize.x * kSize.y * kSize.z * sizeof(double));
		cl::Buffer Y_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, Y_size.x * Y_size.y * Y_size.z * sizeof(double));

		//write the input block to the input buffer
		cl_int err = queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, xSize.x * xSize.y * xSize.z * sizeof(double), x.data());
		printError(err, "queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, xSize.x * xSize.y * xSize.z * sizeof(double), x.data());", __LINE__ - 1, __FILE__, true);
		//write the K block to the K buffer
		err = queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, kSize.x * kSize.y * kSize.z * sizeof(double), k.data());
		printError(err, "queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, kSize.x * kSize.y * kSize.z * sizeof(double), k.data());", __LINE__ - 1, __FILE__, true);

		//create the K
		cl::Kernel Kernel(program, "Convolution3D");

		//set the K arguments
		Kernel.setArg(0, input_buffer);
		Kernel.setArg(1, K_buffer);
		Kernel.setArg(2, Y_buffer);
		Kernel.setArg(3, (int)xSize.x);
		Kernel.setArg(4, (int)xSize.y);
		Kernel.setArg(5, (int)xSize.z);
		Kernel.setArg(6, (int)kSize.x);
		Kernel.setArg(7, (int)kSize.y);
		Kernel.setArg(8, (int)kSize.z);
		Kernel.setArg(9, (int)Y_size.x);
		Kernel.setArg(10, (int)Y_size.y);
		Kernel.setArg(11, (int)Y_size.z);
		Kernel.setArg(12, (int)stride.x);
		Kernel.setArg(13, (int)stride.y);
		Kernel.setArg(14, (int)stride.z);
		Kernel.setArg(15, (int)padding.x);
		Kernel.setArg(16, (int)padding.y);
		Kernel.setArg(17, (int)padding.z);

		//execute the K
		err = queue.enqueueNDRangeKernel(Kernel, cl::NullRange, cl::NDRange(threads.x, threads.y));
		printError(err, "queue.enqueueNDRangeK(K, cl::NullRange, cl::NDRange(threads.x, threads.y), cl::NullRange);", __LINE__ - 1, __FILE__, true);
		//read the Y buffer
		std::vector<double> Y(Y_size.x * Y_size.y * Y_size.z);
		err = queue.enqueueReadBuffer(Y_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * Y_size.z * sizeof(double), Y.data());
		printError(err, "queue.enqueueReadBuffer(Y_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * Y_size.z * sizeof(double), Y.data());", __LINE__ - 1, __FILE__, true);
		
		return Y;
	}

	//FIX THIS!!! for 3d
	std::vector<double> GPUConvolution3DBackProp(std::vector<double> X, std::vector<double>& K, std::vector<double> ZPrime,
		glm::ivec3 X_size, glm::ivec3 K_size, glm::ivec3 Y_size, glm::ivec3 stride, glm::ivec3 padding) {

		//make buffers
		cl::Buffer X_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, X_size.x * X_size.y * sizeof(double));
		cl::Buffer K_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, K_size.x * K_size.y * sizeof(double));
		cl::Buffer ZPrime_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, Y_size.x * Y_size.y * sizeof(double));
		cl::Buffer dY_dX_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, X_size.x * X_size.y * sizeof(double));

		//write the input block to the input buffer
		cl_int err = queue.enqueueWriteBuffer(X_buffer, CL_TRUE, 0, X_size.x * X_size.y * sizeof(double), X.data());
		printError(err, "queue.enqueueWriteBuffer(X_buffer, CL_TRUE, 0, X_size.x * X_size.y * sizeof(double), X.data());", __LINE__ - 1, __FILE__, true);
		//write the K block to the K buffer
		err = queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, K_size.x * K_size.y * sizeof(double), K.data());
		printError(err, "queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, K_size.x * K_size.y * sizeof(double), K.data());", __LINE__ - 1, __FILE__, true);
		//write the Z block to the Z buffer
		err = queue.enqueueWriteBuffer(ZPrime_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * sizeof(double), ZPrime.data());
		printError(err, "queue.enqueueWriteBuffer(ZPrime_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * sizeof(double), ZPrime.data());", __LINE__ - 1, __FILE__, true);

		//create the K
		cl::Kernel Kernel(program, "Convolution2DBackProp");
		//set the K arguments
		Kernel.setArg(0, X_buffer);
		Kernel.setArg(1, K_buffer);
		Kernel.setArg(2, ZPrime_buffer);
		Kernel.setArg(3, dY_dX_buffer);
		Kernel.setArg(4, (int)X_size.x);
		Kernel.setArg(5, (int)X_size.y);
		Kernel.setArg(6, (int)K_size.x);
		Kernel.setArg(7, (int)K_size.y);
		Kernel.setArg(8, (int)Y_size.x);
		Kernel.setArg(9, (int)Y_size.y);
		Kernel.setArg(10, (int)stride.x);
		Kernel.setArg(11, (int)stride.y);
		Kernel.setArg(12, (int)padding.x);
		Kernel.setArg(13, (int)padding.y);

		//execute the K
		err = queue.enqueueNDRangeKernel(Kernel, cl::NullRange, cl::NDRange(threads.x, threads.y));
		printError(err, "queue.enqueueNDRangeK(K, cl::NullRange, cl::NDRange(20, 20), cl::NullRange);", __LINE__ - 1, __FILE__, true);
		//read the K buffer
		err = queue.enqueueReadBuffer(K_buffer, CL_TRUE, 0, K_size.x * K_size.y * sizeof(double), K.data());
		printError(err, "queue.enqueueReadBuffer(K_buffer, CL_TRUE, 0, K_size.x * K_size.y * sizeof(double), K.data());", __LINE__ - 1, __FILE__, true);
		//read the Y buffer,,
		std::vector<double> dY_dX(X_size.x * X_size.y);
		dY_dX.resize(X_size.x * X_size.y);
		err = queue.enqueueReadBuffer(dY_dX_buffer, CL_TRUE, 0, X_size.x * X_size.y * sizeof(double), dY_dX.data());
		printError(err, "queue.enqueueReadBuffer(dY_dX_buffer, CL_TRUE, 0, X_size.x * X_size.y * sizeof(double), dY_dX.data());", __LINE__ - 1, __FILE__, true);

		return dY_dX;
	}

	cl::Program program;
	cl::Context context;
	cl::CommandQueue queue;
	cl::Device device;

	bool GPU = false;
	bool CapDerivative= false;
	double DerivativeCeil = 1.0;
	double DerivativeFloor = -1.0;
public:
	ConvolutionNode() {
		TypeID = "Convolution3DNode";
		//setup gpu
		cl::Platform platform = cl::Platform::getDefault();
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		device = devices[0];
		context = cl::Context(device);
		queue = cl::CommandQueue(context, device);
		cl::Program::Sources sources;
		//convolution K
		std::string K_code;
		//load K code from file
		std::fstream file;
		file.open("Convolution3DKernel.cl", std::ios::in);
		std::string line;
		while (std::getline(file, line)) {
			K_code += line + "\n";
		}
		file.close();

		sources.push_back({ K_code.c_str(),K_code.length() });
		program = cl::Program(context, sources);
		if (program.build({ device }) != CL_SUCCESS) {

			std::fstream file;
			file.open("K_error.txt", std::ios::out);
			file << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
			file.close();
			return;
		}
		K.resize(K_size.x * K_size.y * K_size.x, 0.001);
		RandomizeK();
		//K = BlurK(K_size.x, K_size.y);
		filepath.resize(256);
		SavePath.resize(256);
		//find max thread count for 2d convolution
		int max_threads = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
		printf("Max threads: %d\n", max_threads);
		//sold for the max threads
		//cube root
		threads = glm::ivec3((int)std::round(std::cbrt(max_threads)), (int)std::round(std::cbrt(max_threads)), (int)std::round(std::cbrt(max_threads)));
	}

	glm::ivec3 GetOutputSize(glm::ivec3 input_size, glm::ivec3 K_size, glm::ivec3 stride, glm::ivec3 padding) {
		glm::ivec3 YSize = glm::ivec3(0, 0,0);
		YSize.x = (input_size.x - K_size.x + 2 * padding.x) / stride.x + 1;
		YSize.y = (input_size.y - K_size.y + 2 * padding.y) / stride.y + 1;
		YSize.z = (input_size.z - K_size.z + 2 * padding.z) / stride.z + 1;
		return YSize;
	}

	

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			if (std::filesystem::exists(filepath)) {
				//stb image read into input as color
				int x, y, n;
				unsigned char* data = stbi_load(filepath.data(), &x, &y, &n, STBI_rgb_alpha);
				X_size = glm::ivec3(x, y, n);
				X = std::vector<double>(X_size.x * X_size.y * X_size.z, 0);
				for (int i = 0; i < X.size(); i++) {
					X[i] = (double)data[i] / 255.0;
				}
				stbi_image_free(data);
			}
			else {
				// X_size = { GetInputDataByIndex(0)[0]["Size"]["X"].get<int>(), GetInputDataByIndex(0)[0]["Size"]["Y"].get<int>(), GetInputDataByIndex(0)[0]["Size"]["Z"].get<int>()};
				// X = GetInputDataByIndex(0)[0]["Data"].get<std::vector<double>>();
			}
			Y_size = GetOutputSize(X_size, K_size, stride, padding);
			if (GPU) { 
				Z = GPUConvolution3D(X, K, X_size, K_size, stride, padding, Y_size);
			}
			else {
				//Z = Convolution3D(X, K, X_size, K_size, stride, padding, Y_size);
			}
			//apply activation function
			Y.resize(Z.size());
			nlohmann::json data = nlohmann::json::object();
			data["Width"] = Y_size.x;
			data["Height"] = Y_size.y;
			data["Data"] = nlohmann::json::array();
			data["Type"] = "Matrix";
			for (int i = 0; i < Z.size(); i++)
			{
				Y[i] = Activation->Activate(Z[i]);
				data["Data"].push_back(Y[i]);
			}
			//GetOutputDataByIndex(0) = data;
		}
		else {
			std::vector<double> ZPrime;
			ZPrime.resize(Y_size.x * Y_size.y);
			for (int i = 0; i < ZPrime.size(); i++)
			{
				//ZPrime[i] = Activation->ActivateDerivative(Z[i]) * GetOutputDataByIndex(0)[0]["Data"][i].get<double>() * LearningRate;
			}
			std::vector<double> dY_dX;
			if (GPU) {
				dY_dX = GPUConvolution3DBackProp(X, K, ZPrime, X_size, K_size, Y_size, stride, padding);
			}
			else {
				//dY_dX = Convolution3DBackProp(X, K, ZPrime, X_size, K_size, Y_size, stride, padding);
			}

			//GetInputDataByIndex(0) = nlohmann::json::object();
			//GetInputDataByIndex(0)["Size"]["X"] = X_size.x;
			//GetInputDataByIndex(0)["Size"]["Y"] = X_size.y;
			//GetInputDataByIndex(0)["Data"] = dY_dX;
		}
	}

	void Init() override {
		//Activation = AE->GetAvailableActivations()[0];
		unsigned int input_one=MakeInput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Input");
		});

		unsigned int attribute_one = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Learning Rate", &LearningRate);
			}));

		unsigned int attribute_two = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("GPU?", &GPU);
			}));

		unsigned int attribute_three = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Cap Derivatives?", &CapDerivative);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Ceil", &DerivativeCeil);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Floor", &DerivativeFloor);
			}));

		unsigned int attribute_four = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt3("Kernel Size", &K_size[0])) {
				K.resize(K_size.x * K_size.y * K_size.z, 0.01);
				//RandomizeK();
				//K = BlurK(K_size.x, K_size.y);
			}
			ImGui::SameLine();
			ImGui::InputText("Save Path", SavePath.data(), 256);
			if (ImGui::Button("Save Image")) {
				//save Y as image
				//stb image write
				std::vector<unsigned char> ImageData(Y.size());
				for (int i = 0; i < Y.size(); i++)
				{
					ImageData[i] = (unsigned char)map(Y[i], -1.0, 1.0, 0.0, 255.0);
				}
				stbi_write_png(SavePath.c_str(), Y_size.x, Y_size.y, 1, ImageData.data(), Y_size.x);
			}
			if (ImGui::Button("Randomize Kernel")) {
				RandomizeK();
			}

			ImGui::InputInt3("Stride", &stride[0]);
			ImGui::InputInt3("Padding", &padding[0]);
			ImGui::InputInt3("Threads", &threads[0]);
			ImGui::InputText("Filepath", filepath.data(), filepath.size());
			}));
		unsigned int attribute_five = MakeAttribute(new Attribute([this]() {
			// ImGui::PushItemWidth(100);
			// if (ImGui::BeginCombo("Activation", Activation->GetName().c_str())) {
			// 	for (int i = 0; i < AE->GetAvailableActivations().size(); i++)
			// 	{
			// 		bool selected = false;
			// 		ImGui::Selectable(AE->GetAvailableActivations()[i]->GetName().c_str(), &selected);
			// 		if (selected) {
			// 			Activation = AE->GetAvailableActivations()[i];
			// 		}
			// 	}
			// 	ImGui::EndCombo();
			// }
			}));
		unsigned int attribute_six = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginTable("MyTable", 1)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableNextColumn();

				if (ImGui::CollapsingHeader("Kernel")) {
					//if size is larger than 0 show K
					if (K_size.x > 0 && K_size.y > 0) {
						TensorToTexture(KTexture, K, K_size);
						ImGui::Image((void*)(intptr_t)KTexture, ImVec2(K_size.x, K_size.y));
					}
				}

				ImGui::EndTable();
			}
			}));
		unsigned int attribute_seven = MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginTable("MyTable", 1)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, Y_size.x);
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Output")) {
					//if size is larger than 0 show Y
					if (Y_size.x > 0 && Y_size.y > 0) {
						TensorToTexture(YTexture, Y, Y_size);
						ImGui::Image((void*)(intptr_t)YTexture, ImVec2(Y_size.x, Y_size.y));
					}
				}
				ImGui::EndTable();
			}
			}));
		unsigned int output_one = MakeOutput(NS_DataObject::GetTypeID("Tensor"), []() {
			ImGui::Text("Output");
			});
	}

	void Update(ImGuiContext *Context, GLFWwindow* window) override {
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);

	}

	void DrawNodeTitle(ImGuiContext *Context, GLFWwindow* window){
				//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
	}

	void DrawNodeProperties(ImGuiContext *Context, GLFWwindow* window){
		//set  glfw context
		glfwMakeContextCurrent(window);
		//set imgui context
		ImGui::SetCurrentContext(Context);
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();
		//stride
		data["Stride"]["X"] = stride.x;
		data["Stride"]["Y"] = stride.y;
		data["Stride"]["Z"] = stride.z;

		//padding
		data["Padding"]["X"] = padding.x;
		data["Padding"]["Y"] = padding.y;
		data["Padding"]["Z"] = padding.z;

		//kernel size
		data["KernelSize"]["X"] = K_size.x;
		data["KernelSize"]["Y"] = K_size.y;
		data["KernelSize"]["Z"] = K_size.z;

		//cap derivative
		data["CapDerivative"] = CapDerivative;

		//learning rate
		data["LearningRate"] = LearningRate;

		//kernel weights
		data["K"] = K;

		//activation
		data["Activation"] = Activation->GetName();

		//gpu
		data["GPU"] = GPU;
		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

		//stride
		if (!data["Stride"].is_null()) {
			if ((!data["Stride"]["X"].is_null()) && (!data["Stride"]["Y"].is_null())) {
				stride.x = data["Stride"]["X"].get<int>();
				stride.y = data["Stride"]["Y"].get<int>();
				stride.z = data["Stride"]["Z"].get<int>();
			}
		}
		//padding
		if (!data["Padding"].is_null()) {
			if ((!data["Padding"]["X"].is_null()) && (!data["Padding"]["Y"].is_null())) {
				padding.x = data["Padding"]["X"].get<int>();
				padding.y = data["Padding"]["Y"].get<int>();
				padding.z = data["Padding"]["Z"].get<int>();
			}
		}

		//kernel size
		if (!data["KernelSize"].is_null()) {
			if ((!data["Padding"]["X"].is_null()) && (!data["Padding"]["Y"].is_null())) {
				K_size.x = data["KernelSize"]["X"].get<int>();
				K_size.y = data["KernelSize"]["Y"].get<int>();
				K_size.z = data["KernelSize"]["Z"].get<int>();
			}
		}
		//resize K
		K.resize(K_size.x * K_size.y * K_size.z,0.01);
		RandomizeK();

		//cap derivative
		if (!data["CapDerivative"].is_null()) {
			CapDerivative = data["CapDerivative"].get<bool>();
		}

		//learning rate
		if (!data["LearningRate"].is_null()) {
			LearningRate = data["LearningRate"].get<double>();
		}

		//kernel weights
		if (!data["K"].is_null()) {
			K = data["K"].get<std::vector<double>>();
		}

		//activation
		if (!data["Activation"].is_null()) {
			// for (int i = 0; i < AE->GetAvailableActivations().size(); i++) {
			// 	if (data["Activation"].get<std::string>() == AE->GetAvailableActivations()[i]->GetName()) {
			// 		Activation = AE->GetAvailableActivations()[i];
			// 	}
			// }
		}

		//gpu
		if (!data["GPU"].is_null()) {
			GPU = data["GPU"].get<bool>();
		}

		return;
	}

	NodeInterface* GetInstance() {
		return new ConvolutionNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "Convolution3DNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new ConvolutionNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
