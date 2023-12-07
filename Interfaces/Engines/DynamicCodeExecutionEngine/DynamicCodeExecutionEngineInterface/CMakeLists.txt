project(DynamicCodeExecutionEngineInterface)

add_library(${PROJECT_NAME} STATIC "DynamicCodeExecutionEngineInterface.cpp" "DynamicCodeExecutionEngineInterface.h")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost Boost::filesystem)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)

target_link_libraries(${PROJECT_NAME} PUBLIC EngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC SerializableInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC LibraryInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC ScriptInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC LanguageInterface)