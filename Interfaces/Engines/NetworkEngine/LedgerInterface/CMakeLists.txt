project(LedgerInterface)

add_library(${PROJECT_NAME} INTERFACE "LedgerInterface.h")

# sqlite3
find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${sqlite3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE unofficial::sqlite3::sqlite3)