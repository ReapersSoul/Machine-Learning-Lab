#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>

class FileVisualizationNode : public NodeInterface {
	static const int ImageSize=256;
	std::vector<std::vector<int>> Image;
	std::string FileName;
	GLuint texture;
	enum ScalingType {
		Linear,
		Exponential,
		Logarithmic,
		Boolean,
		Threshold
	}Scaling = Exponential;
	int d_Threshold = 5;

public:
	FileVisualizationNode() {
		TypeID = "FileVisualizationNode";
	}

	void ClearData() {
		for (int i = 0; i < ImageSize; i++)
		{
			for (int j = 0; j < ImageSize; j++)
			{
				Image[i][j] = 0;
			}
		}
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			//clear the image
			ClearData();

			if (!GetInputDataByIndex(0).is_string()) {
				FileName = GetInputDataByIndex(0).get<std::string>();
			}

			//read file in binary mode
			std::ifstream file(FileName, std::ios::binary);
			if (file.fail()) {
				printf("Failed to open file: %s\n", FileName.c_str());
				return;
			}
			//get length of file
			file.seekg(0, file.end);
			int length = file.tellg();
			file.seekg(0, file.beg);

			unsigned char* buffer = new unsigned char[length];

			//read file into buffer
			file.read((char*)buffer, length);
			//close file
			file.close();

			//read file two bytes at a time saving each byte as a int x and y
			for (int i = 0; i < length; i += 2) {
				int x = (int)buffer[i];
				int y = (int)buffer[i + 1];
				Image[x][y]++;
			}

			delete[] buffer;

			//find the max value in the image
			int max = 0;
			for (int i = 0; i < ImageSize; i++) {
				for (int j = 0; j < ImageSize; j++) {
					if (Image[i][j] > max) {
						max = Image[i][j];
					}
				}
			}
			switch (Scaling)
			{
			case FileVisualizationNode::Linear:
				//scale the image to 0-255 using linear scaling
				for (int i = 0; i < ImageSize; i++) {
					for (int j = 0; j < ImageSize; j++) {
						Image[i][j] = (int)(255 * (Image[i][j] / (double)max));
					}
				}
				break;
			case FileVisualizationNode::Exponential:
				//scale the image to 0-255 using exponential scaling
				for (int i = 0; i < ImageSize; i++) {
					for (int j = 0; j < ImageSize; j++) {
						Image[i][j] = (int)(255 * (pow(Image[i][j] / (double)max, 0.5)));
					}
				}
				break;
			case FileVisualizationNode::Logarithmic:
				//scale the image to 0-255 using logarithmic scaling
				for (int i = 0; i < ImageSize; i++) {
					for (int j = 0; j < ImageSize; j++) {
						Image[i][j] = (int)(255 * (log(Image[i][j] / (double)max + 1)));
					}
				}
				break;
			case FileVisualizationNode::Boolean:
				//scale the image to 0-255 using boolean scaling
				for (int i = 0; i < ImageSize; i++) {
					for (int j = 0; j < ImageSize; j++) {
						if (Image[i][j] > 0) {
							Image[i][j] = 255;
						}
						else {
							Image[i][j] = 0;
						}
					}
				}
				break;
			case FileVisualizationNode::Threshold:
				//scale the image to 0-255 using threshold scaling
				for (int i = 0; i < ImageSize; i++) {
					for (int j = 0; j < ImageSize; j++) {
						if (Image[i][j] > d_Threshold) {
							Image[i][j] = 255;
						}
						else {
							Image[i][j] = 0;
						}
					}
				}
				break;
			default:
				break;
			}

			//push image to output
			for (int i = 0; i < ImageSize; i++)
			{
				GetOutputDataByIndex(0).push_back(nlohmann::json::array());
				for (int j = 0; j < ImageSize; j++)
				{
					GetOutputDataByIndex(0)[i].push_back(Image[i][j]);
				}
			}

			std::vector<unsigned char> Pixels(256 * 256, 255);
			Pixels.resize(ImageSize * ImageSize);
			for (int i = 0; i < ImageSize; i++) {
				for (int j = 0; j < ImageSize; j++) {
					Pixels[i + (j * ImageSize)] = (unsigned char)Image[i][j];
				}
			}

			// Create a OpenGL texture identifier
			if(texture!=0)
				glDeleteTextures(1, &texture);

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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ImageSize, ImageSize, 0, GL_RED, GL_UNSIGNED_BYTE, Pixels.data());
		}
	}

	void Init() override {
		FileName.resize(ImageSize);
		Image.resize(ImageSize);
		for (int i = 0; i < ImageSize; i++) {
			Image[i].resize(ImageSize,0);
		}

		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Image((void*)(intptr_t)texture, ImVec2(ImageSize, ImageSize));
			}));

		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::InputText("File Path", (char*)FileName.data(),256);
			}));

		MakeAttribute(1, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Combo("Scaling", (int*)&Scaling, "Linear\0Exponential\0Logarithmic\0Boolean\0Threshold\0");
			if (Scaling == Threshold) {
				ImGui::InputInt("Threshold", &d_Threshold);
			}
			}));

		MakeOutput(0, "Image", "Tensor", nlohmann::json::array());
		MakeInput(0, "File Path", "String", "");
	}

	void StandAloneInit() {

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
		return new FileVisualizationNode();
	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "FileVisualizationNode";
	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new FileVisualizationNode();
	}
}
