project(KinectNode)

add_library(${PROJECT_NAME} SHARED "${PROJECT_NAME}.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)

# Boost::filesystem
find_package(Boost REQUIRED COMPONENTS filesystem)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# glew
find_package(glew REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GLEW_LIBRARIES})

# glfw
find_package(glfw3 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# glm
find_package(glm REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# imgui
find_package(imgui REQUIRED COMPONENTS glfw)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui glfw)

# opengl
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)

# glu
find_package(GLU REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLU_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GLU_LIBRARIES})

# kinect 1.8
find_package(unofficial-kinectsdk1 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${unofficial-kinectsdk1_INCLUDE_DIRS})
#make sure the library exists if not then fatal error
if(EXISTS ${unofficial-kinectsdk1_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} PUBLIC ${unofficial-kinectsdk1_LIBRARIES})
else()
	message(FATAL_ERROR "Kinect 1.8 library not found")
endif()
target_link_directories(${PROJECT_NAME} PUBLIC unofficial-kinectsdk1::unofficial-kinectsdk1)

# get parent path
set(ESCAPE_DEPTH /../../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/ScriptInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/GraphEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/GraphInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/NodeInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/AttributeInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/SorterInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC ScriptInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC NodeInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC AttributeInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC SorterInterface)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/Languages/LanguageInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC LanguageInterface)


# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Lab/Core/Graph/Nodes/
)

