project(GraphInterface)

add_library(${PROJECT_NAME} STATIC "GraphInterface.cpp" "GraphInterface.h")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost Boost::filesystem)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
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
find_package(imgui REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

#imgui_node_editor
find_package(unofficial-imgui-node-editor CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_NODE_EDITOR_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::imgui-node-editor::imgui-node-editor)

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)

target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC PluginInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC LibraryInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC UIEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC EdgeInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC NodeInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC SerializableInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC AttributeInterface)
