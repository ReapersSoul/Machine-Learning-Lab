project(LibraryInterface)

add_library(${PROJECT_NAME} INTERFACE "LibraryInterface.h")

# boost
find_package(boost REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${boost_LIBRARIES})