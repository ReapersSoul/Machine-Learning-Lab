project(GraphEngineInterface)

add_library(${PROJECT_NAME} INTERFACE  "${PROJECT_NAME}.hpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE Boost::boost)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} INTERFACE ${ENGINE_IN_PATH}/GraphEngine/NodeInterface/)
target_link_libraries(${PROJECT_NAME} INTERFACE NodeInterface)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)



