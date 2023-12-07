project(AttributeInterface)

add_library(${PROJECT_NAME} INTERFACE "AttributeInterface.h")

# glew
find_package(glew REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${GLEW_LIBRARIES})

# glfw
find_package(glfw3 REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE glfw)

# glm
find_package(glm REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE glm::glm)

# imgui
find_package(imgui REQUIRED COMPONENTS glfw)
target_include_directories(${PROJECT_NAME} INTERFACE ${IMGUI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE imgui::imgui glfw)

# opengl3
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE OpenGL::GL)