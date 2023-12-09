project(LibraryInterface)

add_library(${PROJECT_NAME} INTERFACE "LibraryInterface.hpp")

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE Boost::boost Boost::filesystem)