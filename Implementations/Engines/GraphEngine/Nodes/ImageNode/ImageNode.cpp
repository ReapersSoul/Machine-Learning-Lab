#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <UIEngineInterface.hpp>
#include <string>

//gl
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//stb_image
#define STB_DEFINE
#if defined(_MSC_VER)
	//  Microsoft 
	#define __STDC_LIB_EXT1__
#endif
#include <stb_include.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


class ImageNode : public NS_Node::NodeInterface {
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
		unsigned int Output_one=MakeOutput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Red");
		});
		unsigned int Output_two=MakeOutput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Green");
		});
		unsigned int Output_three=MakeOutput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Blue");
		});
		unsigned int Output_four=MakeOutput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Image");
		});
		unsigned int Output_five=MakeOutput(NS_DataObject::GetTypeID("Tensor"), [](){
			ImGui::Text("Grey-scale");
		});
		unsigned int Output_six=MakeOutput(NS_DataObject::GetTypeID("Vec2"), [](){
			ImGui::Text("Size");
		});

		unsigned int Attribute_one=MakeAttribute(new Attribute([this]() {
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

		unsigned int Attribute_two=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			ImGui::Checkbox("Lock Aspect Ratio", &lock_aspect_ratio);
			}));

		unsigned int Attribute_three=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt("Width", &desired_image_width, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
				//if the aspect ratio is locked then change the height to match the aspect ratio
				if (lock_aspect_ratio) {
					desired_image_height = (int)(desired_image_width / aspect_ratio);
				}
				ResizeTexture(path.c_str(), desired_image_width, desired_image_height);
			}
			}));

		unsigned int Attribute_four=MakeAttribute(new Attribute([this]() {
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt("Height", &desired_image_height, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
				//if the aspect ratio is locked then change the width to match the aspect ratio
				if (lock_aspect_ratio) {
					desired_image_width = (int)(desired_image_height * aspect_ratio);
				}
				ResizeTexture(path.c_str(), desired_image_width, desired_image_height);
			}
			}));

		unsigned int Attribute_five=MakeAttribute(new Attribute([this]() {
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
		MakeLine(-1, Attribute_one, Output_one);
		MakeLine(-1, Attribute_two, Output_two);
		MakeLine(-1, Attribute_three, Output_three);
		MakeLine(-1, Attribute_four, Output_four);
		MakeLine(-1, Attribute_five, Output_five);
		MakeLine(-1, -1, Output_six);
	}

	void Process(bool DirectionForward) override {
		if (DirectionForward) {
			
		}
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

	NodeInterface* GetInstance() {
		ImageNode* node = new ImageNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);

	}
};


extern "C" {
	EXPORT std::string GetTypeID() {
		return "ImageNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		return new ImageNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}