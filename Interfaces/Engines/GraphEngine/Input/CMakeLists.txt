project("Input")

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE ${PROJECT_NAME}.hpp)

target_include_directories(${PROJECT_NAME} INTERFACE ../../../SerializableInterface/)
target_link_libraries(${PROJECT_NAME} INTERFACE SerializableInterface)