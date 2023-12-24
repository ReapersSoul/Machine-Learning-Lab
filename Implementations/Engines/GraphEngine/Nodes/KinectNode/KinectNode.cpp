#include "../../../../Util/Exports.hpp"
#include <DynamicCodeExecutionEngineInterface.hpp>
#include <NodeInterface.hpp>
#include <LanguageInterface.hpp>
#include <GraphEngineInterface.hpp>
#include <Attribute.hpp>
#include <string>

//kinect nui api and other includes
#include <Windows.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <NuiSkeleton.h>

//gl includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

//kinect global variables
INuiSensor* sensor;
HANDLE colorStreamHandle;
HANDLE depthStreamHandle;
HANDLE skeletonStreamHandle;
HANDLE nextSkeletonEvent;
NUI_SKELETON_FRAME skeletonFrame;
NUI_SKELETON_DATA skeletonData[NUI_SKELETON_COUNT];

GLuint kinectTexture;

//kinect functions
bool initKinect() {
	//get sensor
	int numSensors;
	NuiGetSensorCount(&numSensors);
	if (numSensors < 1) {
		return false;
	}
	NuiCreateSensorByIndex(0, &sensor);

	//init sensor
	sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	HRESULT hr = sensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		NULL,
		&colorStreamHandle
	);
	if (FAILED(hr)) return false;
	//sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480, 0, 2, 0, &depthStreamHandle);
	sensor->NuiSkeletonTrackingEnable(nextSkeletonEvent, 0);

	glGenTextures(1, &kinectTexture);
	glBindTexture(GL_TEXTURE_2D, kinectTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool updateKinect() {
	//update kinect
	sensor->NuiSkeletonGetNextFrame(100, &skeletonFrame);
	NUI_IMAGE_FRAME imageFrame;
	HRESULT hr = sensor->NuiImageStreamGetNextFrame(colorStreamHandle, 120, &imageFrame);
	if (FAILED(hr)) return false;

	// Access color data
	NUI_LOCKED_RECT LockedRect;
	hr = imageFrame.pFrameTexture->LockRect(0, &LockedRect, NULL, 0);
	if (SUCCEEDED(hr))
	{
		// Process or display the color data as needed
		// LockedRect.pBits contains the color image data
		// copy color data to ColorImage
		glBindTexture(GL_TEXTURE_2D, kinectTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 480, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, LockedRect.pBits);
		glBindTexture(GL_TEXTURE_2D, 0);
		// Unlock the frame
		imageFrame.pFrameTexture->UnlockRect(0);
	}

	sensor->NuiImageStreamReleaseFrame(colorStreamHandle, &imageFrame);
	//sensor->NuiImageStreamGetNextFrame(depthStreamHandle, 0, 0);

	//get skeleton data
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		skeletonData[i] = skeletonFrame.SkeletonData[i];
	}
}

std::vector<glm::vec4> GetJoints(std::vector<glm::vec4> oldJoints,int skeleton) {
	if (skeleton == -1) {
		return oldJoints;
	}
	std::vector<glm::vec4> joints=oldJoints;
	joints.resize(NUI_SKELETON_POSITION_COUNT);
	for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		//get joint state
		NUI_SKELETON_POSITION_TRACKING_STATE jointState = skeletonData[skeleton].eSkeletonPositionTrackingState[i];

		// If we can't find the joint, exit
		if (jointState == NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			//continue;
		}

		// If we haven't yet tracked the joint, wait
		if (jointState == NUI_SKELETON_POSITION_INFERRED)
		{
			//continue;
		}
		if (jointState == NUI_SKELETON_POSITION_TRACKED) {
			joints[i] = glm::vec4(skeletonData[skeleton].SkeletonPositions[i].x, skeletonData[skeleton].SkeletonPositions[i].y, skeletonData[skeleton].SkeletonPositions[i].z, skeletonData[skeleton].SkeletonPositions[i].w);
		}
	}
	return joints;
}

static std::string JointNames[] = {
	"Head",
	"ShoulderCenter",
	"ShoulderLeft",
	"ElbowLeft",
	"WristLeft",
	"HandLeft",
	"ShoulderRight",
	"ElbowRight",
	"WristRight",
	"HandRight",
	"Spine",
	"HipCenter",
	"HipLeft",
	"KneeLeft",
	"AnkleLeft",
	"FootLeft",
	"HipRight",
	"KneeRight",
	"AnkleRight",
	"FootRight"
};

class FrameBuffer
{
private:
	GLuint m_fbo = 0;
	GLuint m_rbo = 0;
	GLuint m_ColorTexture = 0;
	int m_width = 0;
	int m_height = 0;
	GLuint last_fbo = 0;
public:
	FrameBuffer() = default;

	FrameBuffer(int width, int height)
	{
		//save last fbo
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&last_fbo);
		GLuint last_rbo;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint*)&last_rbo);
		GLuint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture);


		m_width = width;
		m_height = height;
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(1, &m_ColorTexture);
		glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture, 0);

		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, last_fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, last_rbo);
		glBindTexture(GL_TEXTURE_2D, last_texture);
	}


	//may have to remove causes the texture to be deleted before it can be used
	~FrameBuffer()
	{
		if(m_fbo!=0)
		glDeleteFramebuffers(1, &m_fbo);
		if(m_rbo!=0)
		glDeleteRenderbuffers(1, &m_rbo);
		if(m_ColorTexture!=0)
		glDeleteTextures(1, &m_ColorTexture);
	}

	glm::vec2 getSize()
	{
		return glm::vec2(m_width, m_height);
	}

	void bind()
	{
		//save last fbo
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&last_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, last_fbo);
	}

	GLuint getColorTexture()
	{
		return m_ColorTexture;
	}
};

class KinectNode : public NS_Node::NodeInterface {
	std::vector<glm::vec4> joints;
	float angle = 0;

	FrameBuffer*  SkeletonFrameBuffer;

	void DrawSkeletonTexture(FrameBuffer* SkeletonFrameBuffer) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		SkeletonFrameBuffer->bind();
		glViewport(0, 0, 640, 480);
		glClearColor(0.25f, 0.25f, 0.25f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//perspective projection
		gluPerspective(45.f, 640.f / 480.f, 1.f, 100.f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//view matrix
		gluLookAt(0.f, 0.f, 5.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(-1.f, -1.f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(1.f, -1.f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 1.f, 0.f);
		glEnd();

		/*
		//draw skeleton
		for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
			if (skeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
				for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++) {
					//get joint state
					NUI_SKELETON_POSITION_TRACKING_STATE jointState = skeletonData[i].eSkeletonPositionTrackingState[j];

					// If we can't find the joint, exit
					//if (jointState == NUI_SKELETON_POSITION_NOT_TRACKED)
					//{
					//	continue;
					//}

					//// If we haven't yet tracked the joint, wait
					//if (jointState == NUI_SKELETON_POSITION_INFERRED)
					//{
					//	continue;
					//}

					//get joint position
					float x = skeletonData[i].SkeletonPositions[j].x;
					float y = skeletonData[i].SkeletonPositions[j].y;
					float z = skeletonData[i].SkeletonPositions[j].z;
					float w = skeletonData[i].SkeletonPositions[j].w;

					//draw joint
					glPointSize(10);
					glBegin(GL_POINTS);
					switch (i) {
						case 0:
						glColor3f(1, 0, 0);
						break;
						case 1:
							glColor3f(0, 1, 0);
							break;
						case 2:
							glColor3f(0, 0, 1);
							break;
						case 3:
							glColor3f(1, 1, 0);
							break;
						case 4:
							glColor3f(1, 0, 1);
							break;
						case 5:
							glColor3f(0, 1, 1);
							break;
						case 6:
							glColor3f(1, 1, 1);
							break;
						default:
							glColor3f(0, 0, 0);
							break;
					}

					glVertex2f(x, y);
					glEnd();
				}
			}
		}*/
		SkeletonFrameBuffer->unbind();
		glDisable(GL_DEPTH_TEST);
	}

public:
	KinectNode() {
		TypeID = "KinectNode";
	}

	nlohmann::json GetJointsJson() {
		nlohmann::json data = nlohmann::json::array();
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			nlohmann::json joint;
			joint["x"] = joints[i].x;
			joint["y"] = joints[i].y;
			joint["z"] = joints[i].z;
			joint["w"] = joints[i].w;
			data.push_back(joint);
		}
		return data;
	}

	nlohmann::json GetJointJson(int i) {
		nlohmann::json joint;
		joint["x"] = joints[i].x;
		joint["y"] = joints[i].y;
		joint["z"] = joints[i].z;
		joint["w"] = joints[i].w;
		return joint;
	}

	void Process(bool DirectionForward) override {
		GetOutputDataByIndex(0)=GetJointsJson();
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			GetOutputDataByIndex(i + 1) = GetJointJson(i);
		}
	}

	void Update() override {

		updateKinect();
		//determine which skeleton to use
		int skeleton = -1;
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{
			if (skeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
				skeleton = i;
				break;
			}
		}

		joints = GetJoints(joints, skeleton);
		LONG tmpangle = angle;
		sensor->NuiCameraElevationSetAngle(tmpangle);
	}

	void Init() override {
		SkeletonFrameBuffer= new FrameBuffer(640, 480);
		MakeOutput(0, "Joints", "vec4", nlohmann::json::array());

		joints.resize(NUI_SKELETON_POSITION_COUNT);
		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			joints[i] = glm::vec4(0, 0, 0, 0);
			MakeOutput(i + 1, JointNames[i], "vec4", { 0,0,0,0 });
		}

		MakeAttribute(0, new Attribute([this]() {
			ImGui::Text("Joints: ");
			}));


		for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
		{
			MakeAttribute(i + 1, new Attribute([this, i]() {
				ImGui::Text((JointNames[i] + ": X:" + std::to_string(joints[i].x) + " Y:" + std::to_string(joints[i].y) + " Z:" + std::to_string(joints[i].z) + " W:" + std::to_string(joints[i].w)).c_str());
				}));
		}

		MakeAttribute(NUI_SKELETON_POSITION_COUNT + 2, new Attribute([this]() {
			//angle
			ImGui::PushItemWidth(100);
			ImGui::InputFloat("Angle", &angle, -27, 27);
			}));

		MakeAttribute(NUI_SKELETON_POSITION_COUNT+3, new Attribute([this]() {
			ImGui::Text("Skeleton Image: ");
			DrawSkeletonTexture(SkeletonFrameBuffer);
			ImGui::Image((void*)(intptr_t)SkeletonFrameBuffer->getColorTexture(), ImVec2(SkeletonFrameBuffer->getSize().x, SkeletonFrameBuffer->getSize().y));
			}));
	}

	NodeInterface* GetInstance() {
		KinectNode* node = new KinectNode();
		return node;
	}

	nlohmann::json Serialize() override {
		nlohmann::json data = NS_Node::NodeInterface::Serialize();

		return data;
	}

	void DeSerialize(nlohmann::json data, void* DCEE) override {
		NodeInterface::DeSerialize(data, DCEE);
		return;
	}
};


extern "C" {
	// Define a function that returns the result of adding two numbers
	EXPORT void CleanUp() {
		if (sensor != NULL) {
			sensor->NuiShutdown();
			sensor->Release();
		}
	}

	EXPORT std::string GetTypeID() {
		return "KinectNode";
	}

	// Define a function that returns the result of adding two numbers
	EXPORT NS_Node::NodeInterface* GetInstance() {
		if (sensor == NULL) {
			initKinect();
		}
		return new KinectNode();
	}

	EXPORT void Register(void* registrar) {
        NS_Node::Registrar* Registrar = (NS_Node::Registrar*)registrar;
        Registrar->RegisterNode(GetTypeID(), GetInstance);
    }
}