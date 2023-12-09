project(DebugEngine)

add_library(${PROJECT_NAME} SHARED "DebugEngine.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/Plugins/PluginInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/LibraryInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DebugEngine/DebugEngineInterface/)

target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC PluginInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC LibraryInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC DebugEngineInterface)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)


# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${unofficial-sqlite3_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::sqlite3::sqlite3)