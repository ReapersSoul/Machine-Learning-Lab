project(Cleaner)

add_executable (${PROJECT_NAME} "Cleaner.cpp")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# output
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Cleaner/")

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/Cleaner/Cleaner.exe ARGS ${CMAKE_BINARY_DIR}/Cleaner/Cleaner.exe DEPENDS ${PROJECT_NAME} COMMENT "Running Cleaner")