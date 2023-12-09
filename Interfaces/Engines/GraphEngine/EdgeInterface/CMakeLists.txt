project(EdgeInterface)

add_library(${PROJECT_NAME} STATIC "EdgeInterface.cpp" "EdgeInterface.hpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

target_link_libraries(${PROJECT_NAME} PUBLIC GraphInterface)