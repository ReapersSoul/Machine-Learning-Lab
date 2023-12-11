project(DefaultSorter)

add_library(${PROJECT_NAME} SHARED "DefaultSorter.cpp")

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

# get parent path
set(ESCAPE_DEPTH /../../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/ScriptInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/GraphEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/Graph/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/NodeInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/SorterInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC Graph)
target_link_libraries(${PROJECT_NAME} PUBLIC ScriptInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC NodeInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC SorterInterface)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/Languages/LanguageInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC LanguageInterface)


# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Sorters/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/Graph/Sorters/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Sorters/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/Graph/Sorters/)



