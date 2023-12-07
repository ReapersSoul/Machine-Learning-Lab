#include <DynamicCodeExecutionEngineInterface.h>
#include <NodeInterface.h>
#include <LanguageInterface.h>
#include <GraphEngineInterface.h>
#include <AttributeInterface.h>
#include <UIEngineInterface.h>
#include <string>

//gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//stb_image
#define STB_DEFINE
#define __STDC_LIB_EXT1__
#include <stb_include.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


class ImageNode : public NodeInterface {
	std::string path;
	int image_width = 0;
	int image_height = 0;
	int desired_image_width = 0;
	int desired_image_height = 0;
	GLuint image_texture = 0;
	unsigned char* image_data = nullptr;
	GLuint red_texture = 0;
	unsigned char* red_data = nullptr;
	GLuint green_texture = 0;
	unsigned char* green_data = nullptr;
	GLuint blue_texture = 0;
	unsigned char* blue_data = nullptr;
	GLuint grey_texture = 0;
	unsigned char* grey_data = nullptr;
	float aspect_ratio = 1.0f;
	bool lock_aspect_ratio = true;
	int display_mode = 0;
public:
	ImageNode() {
		TypeID = "ImageNode";
	}

	nlohmann::json AsTensor(unsigned char* data, int width, int height,int channels) {
		nlohmann::json ret;
		ret["Size"]["X"] = width;
		ret["Size"]["Y"] = height;
		ret["Size"]["Z"] = channels;
		for (int i = 0; i < width*height*channels; i++) {
			ret["Data"].push_back((double)((double)data[i] / 255.0));
		}
		ret["Type"] = "Tensor";
		return ret;
	}

	bool LoadTextureFromFile(const char* filename)
	{
		if (image_data != nullptr) {
			stbi_image_free(image_data);
			delete[] red_data;
			delete[] green_data;
			delete[] blue_data;
			delete[] grey_data;
		}
		// Load from file
		image_data = stbi_load(filename, &image_width, &image_height, NULL, 3);
		if (image_data == NULL)
			return false;

		//load the image data into the red green and blue data
		red_data = new unsigned char[image_width * image_height];
		green_data = new unsigned char[image_width * image_height];
		blue_data = new unsigned char[image_width * image_height];
		grey_data = new unsigned char[image_width * image_height];

		for (int i = 0; i < image_width * image_height; i++) {
			red_data[i] = image_data[i * 3];
			green_data[i] = image_data[i * 3 + 1];
			blue_data[i] = image_data[i * 3 + 2];
			grey_data[i] = (unsigned char)((image_data[i * 3] + image_data[i * 3 + 1] + image_data[i * 3 + 2]) / 3);
		}

		//Image
		// Create a OpenGL texture identifier
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

		//Red
		// Create a OpenGL texture identifier
		glGenTextures(1, &red_texture);
		glBindTexture(GL_TEXTURE_2D, red_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, red_data);

		//Green
		// Create a OpenGL texture identifier
		glGenTextures(1, &green_texture);
		glBindTexture(GL_TEXTURE_2D, green_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_GREEN, GL_UNSIGNED_BYTE, green_data);

		//Blue
		// Create a OpenGL texture identifier
		glGenTextures(1, &blue_texture);
		glBindTexture(GL_TEXTURE_2D, blue_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_BLUE, GL_UNSIGNED_BYTE, blue_data);

		//Grey-scale
		// Create a OpenGL texture identifier
		glGenTextures(1, &grey_texture);
		glBindTexture(GL_TEXTURE_2D, grey_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, grey_data);

		return true;
	}

	bool ResizeTexture(const char* filename, int new_width, int new_height)
	{
		//reload the image data
		LoadTextureFromFile(filename);
		//resize image using stbi_resize
		unsigned char* resized_image_data = new unsigned char[new_width * new_height * 3];
		stbir_resize_uint8(image_data, image_width, image_height, 0, resized_image_data, new_width, new_height, 0, 3);
		//free the old image data
		stbi_image_free(image_data);
		//delete the old red green and blue data
		delete[] red_data;
		delete[] green_data;
		delete[] blue_data;
		delete[] grey_data;
		
		image_data = resized_image_data;
		image_width = new_width;
		image_height = new_height;
		//load the image data into the red green and blue data
		red_data = new unsigned char[image_width * image_height];
		green_data = new unsigned char[image_width * image_height];
		blue_data = new unsigned char[image_width * image_height];
		grey_data = new unsigned char[image_width * image_height];

		for (int i = 0; i < image_width * image_height; i++) {
			red_data[i] = image_data[i * 3];
			green_data[i] = image_data[i * 3 + 1];
			blue_data[i] = image_data[i * 3 + 2];
			grey_data[i] = (unsigned char)((image_data[i * 3] + image_data[i * 3 + 1] + image_data[i * 3 + 2]) / 3);
		}
		//if there is an old texture delete it
		if (image_texture != 0) {
			glDeleteTextures(1, &image_texture);
		}
		if (red_texture != 0) {
			glDeleteTextures(1, &red_texture);
		}
		if (green_texture != 0) {
			glDeleteTextures(1, &green_texture);
		}
		if (blue_texture != 0) {
			glDeleteTextures(1, &blue_texture);
		}
		if (grey_texture != 0) {
			glDeleteTextures(1, &grey_texture);
		}

		//Image
		// Create a OpenGL texture identifier
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);

		//Red
		// Create a OpenGL texture identifier
		glGenTextures(1, &red_texture);
		glBindTexture(GL_TEXTURE_2D, red_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, red_data);

		//Green
		// Create a OpenGL texture identifier
		glGenTextures(1, &green_texture);
		glBindTexture(GL_TEXTURE_2D, green_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_GREEN, GL_UNSIGNED_BYTE, green_data);

		//Blue
		// Create a OpenGL texture identifier
		glGenTextures(1, &blue_texture);
		glBindTexture(GL_TEXTURE_2D, blue_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_BLUE, GL_UNSIGNED_BYTE, blue_data);

		//Grey-scale
		// Create a OpenGL texture identifier
		glGenTextures(1, &grey_texture);
		glBindTexture(GL_TEXTURE_2D, grey_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Upload pixels into texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, grey_data);

		return true;
	}

	void Init() override {
		path.reserve(256);
		MakeOutput(0, "Red", "Tensor", nlohmann::json::array());
		MakeOutput(1, "Green", "Tensor", nlohmann::json::array());
		MakeOutput(2, "Blue", "Tensor", nlohmann::json::array());
		MakeOutput(3, "Image", "Tensor", nlohmann::json::array());
		MakeOutput(4, "Grey-scale", "Tensor", nlohmann::json::array());
		MakeOutput(5, "Size", "vec2", nlohmann::json::array());

		MakeAttribute(0, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			char* tmppath = new char[256] {0};
			tmppath = (char*)path.c_str();
			ImGui::InputText("Path", tmppath, 256);
			if (path != tmppath) {
				path = tmppath;
				bool ret = LoadTextureFromFile(path.c_str());
				if (ret) {
					aspect_ratio= (float)image_width / (float)image_height;
					desired_image_height = image_height;
					desired_image_width = image_width;
					ResizeTexture(path.c_str(), desired_image_width, desired_image_height);
				}
			}
			}));

		MakeAttribute(1, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Lock Aspect Ratio", &lock_aspect_ratio);
			}));

		MakeAttribute(2, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt("Width", &desired_image_width, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
				//if the aspect ratio is locked then change the height to match the aspect ratio
				if (lock_aspect_ratio) {
					desired_image_height = (int)(desired_image_width / aspect_ratio);
				}
				ResizeTexture(path.c_str(), desired_image_width, desired_image_height);
			}
			}));

		MakeAttribute(3, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt("Height", &desired_image_height, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
				//if the aspect ratio is locked then change the width to match the aspect ratio
				if (lock_aspect_ratio) {
					desired_image_width = (int)(desired_image_height * aspect_ratio);
				}
				ResizeTexture(path.c_str(), desired_image_width, desired_image_height);
			}
			}));

		MakeAttribute(6, new AttributeInterface([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::CollapsingHeader("Image")) {
				//radio buttons for the display mode
				if (ImGui::RadioButton("Image", display_mode == 0)) {
					display_mode = 0;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Red", display_mode == 1)) {
					display_mode = 1;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Green", display_mode == 2)) {
					display_mode = 2;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Blue", display_mode == 3)) {
					display_mode = 3;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("Grey-scale", display_mode == 4)) {
					display_mode = 4;
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("None", display_mode == 5)) {
					display_mode = 5;
				}
				switch (display_mode)
				{
				case 0:
					ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
					break;
				case 1:
					ImGui::Image((void*)(intptr_t)red_texture, ImVec2(image_width, image_height));
					break;
				case 2:
					ImGui::Image((void*)(intptr_t)green_texture, ImVec2(image_width, image_height));
					break;
				case 3:
					ImGui::Image((void*)(intptr_t)blue_texture, ImVec2(image_width, image_height));
					break;
				case 4:
					ImGui::Image((void*)(intptr_t)grey_texture, ImVec2(image_width, image_height));
					break;
				default:
					break;
				}
			}
			}));
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {

			//Outputs[0]->GetData().push_back(AsTensor(red_data, image_width, image_height));
			//Outputs[1]->GetData().push_back(AsTensor(green_data, image_width, image_height));
			//Outputs[2]->GetData().push_back(AsTensor(blue_data, image_width, image_height));
			//Outputs[3]->GetData().push_back(AsTensor(image_data, image_width, image_height));
			//Outputs[4]->GetData().push_back(AsTensor(grey_data, image_width, image_height));
			GetOutputDataByIndex(0) = AsTensor(red_data, image_width, image_height,1);
			GetOutputDataByIndex(1) = AsTensor(green_data, image_width, image_height,1);
			GetOutputDataByIndex(2) = AsTensor(blue_data, image_width, image_height,1);
			GetOutputDataByIndex(3) = AsTensor(image_data, image_width, image_height,1);
			GetOutputDataByIndex(4) = AsTensor(grey_data, image_width, image_height,1);
		}
	}

	//void DrawNodeTitle();

	NodeInterface* GetInstance() {
		ImageNode* node = new ImageNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

	}
};


extern "C" {
	__declspec(dllexport) std::string GetTypeID() {
		return "ImageNode";
	}

	// Define a function that returns the result of adding two numbers
	__declspec(dllexport) NodeInterface* GetInstance() {
		return new ImageNode();
	}
}