project(Cleaner)

add_executable (${PROJECT_NAME} "Cleaner.cpp")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# output
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Runtime/Debug/Cleaner/")

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)