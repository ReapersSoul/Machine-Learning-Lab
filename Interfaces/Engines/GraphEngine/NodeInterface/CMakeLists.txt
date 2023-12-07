project(NodeInterface)

add_library(${PROJECT_NAME} STATIC "NodeInterface.cpp" "NodeInterface.h")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

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

#imgui_node_editor
find_package(unofficial-imgui-node-editor CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_NODE_EDITOR_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::imgui-node-editor::imgui-node-editor)

# opengl
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)

target_link_libraries(${PROJECT_NAME} PUBLIC SerializableInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC EdgeInterface)
