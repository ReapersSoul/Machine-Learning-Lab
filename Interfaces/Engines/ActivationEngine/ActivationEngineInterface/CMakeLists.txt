project(ActivationEngineInterface)

add_library(${PROJECT_NAME} INTERFACE "ActivationEngineInterface.hpp")

target_link_libraries(${PROJECT_NAME} INTERFACE ActivationInterface)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE Boost::boost Boost::filesystem)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)