project(NetworkObjectInterface)

add_library(${PROJECT_NAME} INTERFACE "NetworkObjectInterface.hpp")

# nlohmann-json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

# boost beast
find_package(Boost REQUIRED COMPONENTS system)
target_include_directories(${PROJECT_NAME} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${Boost_LIBRARIES})

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

# DynamicCodeExecutionEngineInterface
target_include_directories(${PROJECT_NAME} INTERFACE ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_link_libraries(${PROJECT_NAME} INTERFACE DynamicCodeExecutionEngineInterface)