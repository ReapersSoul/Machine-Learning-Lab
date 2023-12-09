project(ClientInterface)

add_library(${PROJECT_NAME} INTERFACE "ClientInterface.hpp")

# nlohmann-json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)