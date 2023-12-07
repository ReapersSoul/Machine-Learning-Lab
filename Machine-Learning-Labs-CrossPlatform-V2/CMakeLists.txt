project(Machine-Learning-Labs-CrossPlatform-V2 VERSION 2.0)

add_executable (Machine-Learning-Labs-CrossPlatform-V2 "Machine-Learning-Labs-CrossPlatform-V2.cpp" "Machine-Learning-Labs-CrossPlatform-V2.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Machine-Learning-Labs-CrossPlatform-V2 PROPERTY CXX_STANDARD 20)
endif()

# Version File
configure_file(MLL_version.h.in MLL_version.h)

#boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} PUBLIC Boost::filesystem Boost::system)

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

# opengl3
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::GL)

# nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Interfaces/)
set(ENGINE_PATH ${INTERFACE_PATH}/Engines/)

# DynamicCodeExecutionEngineInterface
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
# UIEngine
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_PATH}/UIEngine/UIEngineInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC UIEngineInterface)
# NetworkEngine
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_PATH}/NetworkEngine/NetworkEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_PATH}/NetworkEngine/LedgerInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC NetworkEngineInterface)
# Serializable interface
target_include_directories(${PROJECT_NAME} PUBLIC ${INTERFACE_PATH}/SerializableInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC SerializableInterface)