project(DependancyGraphMaker)

add_executable(${PROJECT_NAME} "${PROJECT_NAME}.cpp")

# c++ 20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${unofficial-sqlite3_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::sqlite3::sqlite3)

# output
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "<1:${CMAKE_BINARY_DIR}/Runtime/DependancyGraphMaker>")