project(LanguageInterface)

add_library(${PROJECT_NAME} INTERFACE "LanguageInterface.hpp")

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Plugins/Languages/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Plugins/Languages/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Plugins/Languages/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Plugins/Languages/)

