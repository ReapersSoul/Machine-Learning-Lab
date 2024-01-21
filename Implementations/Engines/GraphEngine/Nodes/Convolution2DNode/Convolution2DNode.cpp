#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>
#include <thread>
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

class Convolution2DNode : public NS_Node::NodeInterface {
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

	glm::ivec2 X_size{0, 0};
	glm::ivec2 K_size{3, 3};
	glm::ivec2 stride{1, 1};
	glm::ivec2 padding{2, 2};
	glm::ivec2 Y_size{0, 0};
	glm::ivec2 threads{1000, 1000};

	std::string filepath;
	std::string SavePath;

	bool CapDerivative = false;
	double DerivativeCeil = 1;
	double DerivativeFloor = 0;

	bool GPU = false;

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

	void MatrixToTexture(GLuint &texture,std::vector<double> matrix, glm::ivec2 size) {
		//check if texture exists
		if (texture != 0) {
			glDeleteTextures(1, &texture);
		}

		std::vector<unsigned char> ImageData;
		ImageData.resize(matrix.size());
		for (int i = 0; i < matrix.size(); i++)
		{
			ImageData[i] = (unsigned char)map(matrix[i], -1.0, 1.0, 0.0, 255.0);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, ImageData.data());
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

	void printError(cl_int error, const char* function, int line,const char* file, bool should_throw = false){
		if (error != CL_SUCCESS) {
			printf("Error: %s\nWhile: %s\nLine: %d\nFile: %s\n", getErrorString(error), function, line, file);
			if (should_throw) {
				//throw custom exception
				throw CustomException("Error: " + std::string(getErrorString(error)) + "\nWhile: " + std::string(function) + "\nLine: " + std::to_string(line) + "\nFile: " + std::string(file));
			}
		}
	}

	std::vector<double> Convolution2D(std::vector<double> X, std::vector<double> K, glm::ivec2 xSize, glm::ivec2 kSize, glm::ivec2 stride, glm::ivec2 padding, glm::ivec2 Y_size) {
		//check cpu thread count
		int threadCount = sqrt(std::thread::hardware_concurrency());
		int threadCount_x = threadCount;
		int threadCount_y = threadCount;
		if (threadCount_x > Y_size.x) {
			threadCount_x = Y_size.x;
		}
		if (threadCount_y > Y_size.y) {
			threadCount_y = Y_size.y;
		}

		//start spawning threads
		std::vector<std::thread> threads;

		//create output vector
		std::vector<double> Y(Y_size.x * Y_size.y);

		for (int gid_x = 0; gid_x < threadCount_x; gid_x++) {
			for (int gid_y = 0; gid_y < threadCount_y; gid_y++) {
				threads.push_back(std::thread([gid_x,gid_y,Y_size,threadCount_x,threadCount_y,stride,padding,xSize,kSize,&Y,X,K](){
					// Starting index of the output for this thread
					int StartingOutput_x = gid_x * (Y_size.x / threadCount_x);
					int StartingOutput_y = gid_y * (Y_size.y / threadCount_y);

					// Number of outputs to calculate for this thread
					int OutputsToCalculate_x = (Y_size.x / threadCount_x);
					int OutputsToCalculate_y = (Y_size.y / threadCount_y);

					if (gid_x == threadCount_x - 1) {
						OutputsToCalculate_x += Y_size.x % threadCount_x;
					}
					if (gid_y == threadCount_y - 1) {
						OutputsToCalculate_y += Y_size.y % threadCount_y;
					}

					for (int x = StartingOutput_x; x < StartingOutput_x + OutputsToCalculate_x; x++) {
						for (int y = StartingOutput_y; y < StartingOutput_y + OutputsToCalculate_y; y++) {
							int index = y * Y_size.x + x;
							Y[index] = 0;

							for (int k = 0; k < kSize.x; k++){
								for (int l = 0; l < kSize.y; l++) {
									int kernelIndex = l * kSize.x + k;

									// Calculate the input index based on the output and kernel positions
									int InputIndexX = x * stride.x - padding.x + k;
									int InputIndexY = y * stride.y - padding.y + l;

									// Check if the input index is within the bounds of the input data
									if (InputIndexX >= 0 && InputIndexX < xSize.x && InputIndexY >= 0 && InputIndexY < xSize.y) {
										int InputIndex = InputIndexY * xSize.x + InputIndexX;
										Y[index] += X[InputIndex] * K[kernelIndex];
									}
								}
							}
						}
					}
					}));
			}
		}

		//wait for all threads to finish
		for (int i = 0; i < threads.size(); i++) {
			threads[i].join();
		}

		return Y;		
	}

	std::vector<double> Convolution2DBackProp(std::vector<double> X, std::vector<double>& K, std::vector<double> ZPrime,
		glm::ivec2 X_size, glm::ivec2 K_size, glm::ivec2 Y_size, glm::ivec2 stride, glm::ivec2 padding) {
		//check cpu thread count
		int threadCount = sqrt(std::thread::hardware_concurrency());
		int threadCount_x = threadCount;
		int threadCount_y = threadCount;
		if (threadCount_x > Y_size.x) {
			threadCount_x = Y_size.x;
		}
		if (threadCount_y > Y_size.y) {
			threadCount_y = Y_size.y;
		}

		//start spawning threads
		std::vector<std::thread> threads;

		//create output vector
		std::vector<double> dY_dX(X_size.x * X_size.y);

		for (int gid_x = 0; gid_x < threadCount_x; gid_x++) {
			for (int gid_y = 0; gid_y < threadCount_y; gid_y++) {
				threads.push_back(std::thread([&,gid_x, gid_y, Y_size, threadCount_x, threadCount_y, stride, padding,X_size,K_size,X,ZPrime](){
					// Starting index of the output for this thread
					int StartingOutput_x = gid_x * (Y_size.x / threadCount_x);
					int StartingOutput_y = gid_y * (Y_size.y / threadCount_y);

					// Number of outputs to calculate for this thread
					int OutputsToCalculate_x = (Y_size.x / threadCount_x);
					int OutputsToCalculate_y = (Y_size.y / threadCount_y);

					if (gid_x == threadCount_x - 1) {
						OutputsToCalculate_x += Y_size.x % threadCount_x;
					}
					if (gid_y == threadCount_y - 1) {
						OutputsToCalculate_y += Y_size.y % threadCount_y;
					}

					for (int i = StartingOutput_x; i < StartingOutput_x + OutputsToCalculate_x; i++) {
						for (int y = StartingOutput_y; y < StartingOutput_y + OutputsToCalculate_y; y++) {
							for (int k = 0; k < K_size.x; k++) {
								for (int l = 0; l < K_size.y; l++) {
									int kernelIndex = l * K_size.x + k;

									// Calculate the input index based on the output and kernel positions
									int InputIndexX = i * stride.x - padding.x + k;
									int InputIndexY = y * stride.y - padding.y + l;

									// Check if the input index is within the bounds of the input data
									if (InputIndexX >= 0 && InputIndexX < X_size.x && InputIndexY >= 0 && InputIndexY < X_size.y) {
										int InputIndex = InputIndexY * X_size.x + InputIndexX;
										dY_dX[InputIndex] += ZPrime[y * Y_size.x + i] * K[kernelIndex];
										K[kernelIndex] += ZPrime[y * Y_size.x + i] * X[InputIndex];
										if (CapDerivative) {
											if (fabs(K[kernelIndex]) > DerivativeCeil){
												K[kernelIndex] = DerivativeCeil;
											}
											if (fabs(K[kernelIndex]) < DerivativeFloor){
												K[kernelIndex] = DerivativeFloor;
											}
											if (fabs(dY_dX[InputIndex]) > DerivativeCeil){
												dY_dX[InputIndex] = DerivativeCeil;
											}
											if (fabs(dY_dX[InputIndex]) < DerivativeFloor){
												dY_dX[InputIndex] = DerivativeFloor;
											}
										}
									}
								}
							}
						}
					}
				}));
			}
		}

		//wait for all threads to finish
		for (int i = 0; i < threads.size(); i++) {
			threads[i].join();
		}

		return dY_dX;
	}

	std::vector<double> GPUConvolution2D(std::vector<double> x, std::vector<double> k, glm::ivec2 xSize, glm::ivec2 kSize, glm::ivec2 stride, glm::ivec2 padding,glm::ivec2 Y_size) {
		//make buffers
		cl::Buffer input_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, xSize.x * xSize.y * sizeof(double));
		cl::Buffer K_buffer = cl::Buffer(context, CL_MEM_READ_ONLY, kSize.x * kSize.y * sizeof(double));
		cl::Buffer Y_buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, Y_size.x * Y_size.y * sizeof(double));
		
		//write the input block to the input buffer
		cl_int err=queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, xSize.x * xSize.y * sizeof(double), x.data());
		printError(err, "queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, x.size() * x[0].size() * sizeof(double), x.data());", __LINE__-1, __FILE__, true);
		//write the K block to the K buffer
		err = queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, kSize.x * kSize.y * sizeof(double), k.data());
		printError(err, "queue.enqueueWriteBuffer(K_buffer, CL_TRUE, 0, k.size() * k[0].size() * sizeof(double), k.data());", __LINE__-1, __FILE__, true);

		//create the K
		cl::Kernel Kernel(program, "Convolution2D");

		//set the K arguments
		Kernel.setArg(0, input_buffer);
		Kernel.setArg(1, K_buffer);
		Kernel.setArg(2, Y_buffer);
		Kernel.setArg(3, (int)xSize.x);
		Kernel.setArg(4, (int)xSize.y);
		Kernel.setArg(5, (int)kSize.x);
		Kernel.setArg(6, (int)kSize.y);
		Kernel.setArg(7, (int)Y_size.x);
		Kernel.setArg(8, (int)Y_size.y);
		Kernel.setArg(9, (int)stride.x);
		Kernel.setArg(10, (int)stride.y);
		Kernel.setArg(11, (int)padding.x);
		Kernel.setArg(12, (int)padding.y);

		//execute the K
		err = queue.enqueueNDRangeKernel(Kernel, cl::NullRange, cl::NDRange(threads.x, threads.y));
		printError(err, "queue.enqueueNDRangeK(K, cl::NullRange, cl::NDRange(threads.x, threads.y), cl::NullRange);", __LINE__-1, __FILE__, true);
		//read the Y buffer
		std::vector<double> Y(Y_size.x * Y_size.y);
		err = queue.enqueueReadBuffer(Y_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * sizeof(double), Y.data());
		printError(err, "queue.enqueueReadBuffer(Y_buffer, CL_TRUE, 0, Y_size.x * Y_size.y * sizeof(double), Y_block.data());", __LINE__-1, __FILE__, true);

		return Y;
	}

	std::vector<double> GPUConvolution2DBackProp(std::vector<double> X, std::vector<double>& K, std::vector<double> ZPrime,
		glm::ivec2 X_size, glm::ivec2 K_size, glm::ivec2 Y_size, glm::ivec2 stride, glm::ivec2 padding) {

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
		Kernel.setArg(14, (int)CapDerivative);
		Kernel.setArg(15, (double)DerivativeCeil);
		Kernel.setArg(16, (double)DerivativeFloor);

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
public:
	Convolution2DNode() {
		//setup gpu
		cl::Platform platform = cl::Platform::getDefault();
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
		device = devices[0];
		context= cl::Context(device);
		queue= cl::CommandQueue(context, device);
		cl::Program::Sources sources;
		//convolution K
		std::string K_code;
		//load K code from file
		std::fstream file;
		file.open("Convolution2DKernel.cl", std::ios::in);
		std::string line;
		while (std::getline(file, line)) {
			K_code += line + "\n";
		}
		file.close();

		sources.push_back({ K_code.c_str(),K_code.length() });
		program= cl::Program(context, sources);
		if (program.build({ device }) != CL_SUCCESS) {

			std::fstream file;
			file.open("K_error.txt", std::ios::out);
			file << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
			file.close();
			return;
		}
		TypeID = "Convolution2DNode";
		K.resize(K_size.x*K_size.y, 0.001);
		RandomizeK();
		//K = BlurK(K_size.x, K_size.y);
		filepath.resize(256);
		SavePath.resize(256);
		//find max thread count for 2d convolution
		int max_threads = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
		printf("Max threads: %d\n", max_threads);
		//sold for the max threads
		threads = glm::ivec2(sqrt(max_threads), sqrt(max_threads));
	}


	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			if (std::filesystem::exists(filepath)) {
				//stb image read into input as greyscale
				int x, y, n;
				unsigned char* data = stbi_load(filepath.data(), &x, &y, &n, STBI_grey);
				X_size = glm::ivec2(x, y);
				X = std::vector<double>(X_size.x * X_size.y, 0);
				for (int i = 0; i < X.size(); i++){
					X[i] = (double)data[i] / 255.0;
				}
				stbi_image_free(data);
			}
			else {
				// X_size = {GetInputDataByIndex(0)[0]["Width"].get<int>(), GetInputDataByIndex(0)[0]["Height"].get<int>()};
				// X = GetInputDataByIndex(0)[0]["Data"].get<std::vector<double>>();
			}
			Y_size = GetOutputSize(X_size, K_size, stride, padding);
			if (GPU) {
				Z = GPUConvolution2D(X, K, X_size, K_size, stride, padding, Y_size);
			}
			else {
				Z = Convolution2D(X, K, X_size, K_size, stride, padding, Y_size);
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
				dY_dX = GPUConvolution2DBackProp(X, K, ZPrime, X_size, K_size, Y_size, stride, padding);
			}
			else {
				dY_dX = Convolution2DBackProp(X, K, ZPrime, X_size, K_size, Y_size, stride, padding);
			}
			
			// GetInputDataByIndex(0) = nlohmann::json::object();
			// GetInputDataByIndex(0)["Size"]["X"] = X_size.x;
			// GetInputDataByIndex(0)["Size"]["Y"] = X_size.y;
			// GetInputDataByIndex(0)["Data"] = dY_dX;
		}
	}

	void Init() override {
		//Activation = AE->GetAvailableActivations()[0];
		unsigned int input_one = MakeInput(NS_DataObject::GetTypeID("Tensor"),[](){
			ImGui::Text("Input");
		});

		unsigned int attribute_one=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputDouble("Learning Rate", &LearningRate);
			}));

		unsigned int attribute_two=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("GPU?", &GPU);
			}));

		unsigned int attribute_three=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Cap Derivatives?", &CapDerivative);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Ceil", &DerivativeCeil);
			ImGui::SameLine();
			ImGui::InputDouble("Derivative Floor", &DerivativeFloor);
		}));

		unsigned int attribute_four=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt2("Kernel Size", &K_size[0])) {
				K.resize(K_size.x * K_size.y, 0.001);
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

			ImGui::InputInt2("Stride", &stride[0]);
			ImGui::InputInt2("Padding", &padding[0]);
			ImGui::InputInt2("Threads", &threads[0]);
			ImGui::InputText("Filepath", filepath.data(), filepath.size());
			}));
		unsigned int attribute_five=MakeAttribute(new Attribute([this]() {
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
		unsigned int attribute_six=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginTable("MyTable", 1)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableNextColumn();

				if (ImGui::CollapsingHeader("Kernel")) {
					//if size is larger than 0 show K
					if (K_size.x > 0 && K_size.y > 0) {
						MatrixToTexture(KTexture, K, K_size);
						ImGui::Image((void*)(intptr_t)KTexture, ImVec2(K_size.x, K_size.y));
					}
				}

				ImGui::EndTable();
			}
			}));
		unsigned int attribute_seven=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::BeginTable("MyTable", 1)) {
				ImGui::TableSetupColumn("Column 1", ImGuiTableColumnFlags_WidthFixed, Y_size.x);
				ImGui::TableNextColumn();
				if (ImGui::CollapsingHeader("Output")) {
					//if size is larger than 0 show Y
					if (Y_size.x > 0 && Y_size.y > 0) {
						MatrixToTexture(YTexture, Y, Y_size);
						ImGui::Image((void*)(intptr_t)YTexture, ImVec2(Y_size.x, Y_size.y));
					}
				}
				ImGui::EndTable();
			}
			}));
		unsigned int output_one=MakeOutput(NS_DataObject::GetTypeID("Tensor"),[]() {
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

		//padding
		data["Padding"]["X"] = padding.x;
		data["Padding"]["Y"] = padding.y;

		//kernel size
		data["KernelSize"]["X"] = K_size.x;
		data["KernelSize"]["Y"] = K_size.y;

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
			}
		}
		//padding
		if (!data["Padding"].is_null()) {
			if ((!data["Padding"]["X"].is_null()) && (!data["Padding"]["Y"].is_null())) {
				padding.x = data["Padding"]["X"].get<int>();
				padding.y = data["Padding"]["Y"].get<int>();
			}
		}

		//kernel size
		if (!data["KernelSize"].is_null()) {
			if ((!data["Padding"]["X"].is_null()) && (!data["Padding"]["Y"].is_null())) {
				K_size.x = data["KernelSize"]["X"].get<int>();
				K_size.y = data["KernelSize"]["Y"].get<int>();
			}
		}
		//resize K
		K.resize(K_size.x * K_size.y, 0.001);
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
		// if (!data["Activation"].is_null()) {
		// 	for (int i = 0; i < AE->GetAvailableActivations().size(); i++) {
		// 		if (data["Activation"].get<std::string>() == AE->GetAvailableActivations()[i]->GetName()) {
		// 			Activation = AE->GetAvailableActivations()[i];
		// 		}
		// 	}
		// }

		//gpu
		if (!data["GPU"].is_null()) {
			GPU = data["GPU"].get<bool>();
		}

		return;
	}

	NodeInterface* GetInstance() {
		return new Convolution2DNode();
	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "Convolution2DNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new Convolution2DNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}
