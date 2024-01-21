project(LedgerInterface)

add_library(${PROJECT_NAME} INTERFACE "LedgerInterface.hpp")

# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${sqlite3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE unofficial::sqlite3::sqlite3)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Runtime/Debug/Machine-Learning-Lab/Core/)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Runtime/Release/Machine-Learning-Lab/Core/)



