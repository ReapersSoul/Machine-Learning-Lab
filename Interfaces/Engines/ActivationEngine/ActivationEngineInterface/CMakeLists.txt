project(ActivationEngineInterface)

add_library(${PROJECT_NAME} INTERFACE "ActivationEngineInterface.hpp")

target_include_directories(${PROJECT_NAME} INTERFACE ${ENGINE_IN_PATH}/ActivationEngine/ActivationInterface/)
target_link_libraries(${PROJECT_NAME} INTERFACE ActivationInterface)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)
