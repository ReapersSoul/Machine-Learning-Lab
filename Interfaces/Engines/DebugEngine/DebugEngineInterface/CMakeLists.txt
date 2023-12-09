project(DebugEngineInterface)

add_library(${PROJECT_NAME} INTERFACE "DebugEngineInterface.hpp")

target_link_libraries(${PROJECT_NAME} INTERFACE EngineInterface)

# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${unofficial-sqlite3_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE unofficial::sqlite3::sqlite3)