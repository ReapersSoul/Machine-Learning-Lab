project(JavaScript_Language)

add_library(${PROJECT_NAME} SHARED "JavaScript_Language.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# get parent path
set(ESCAPE_DEPTH /../../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/ScriptInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/Languages/LanguageInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/Nodes/NodeInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/Attributes/AttributeInterface/)

# duktape
find_package(duktape REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${duktape_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${duktape_LIBRARIES})

# OpenCl
# find_package(OpenCL REQUIRED)
# target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
# find_package(Boost REQUIRED)
# target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
# Boost::filesystem
# find_package(Boost REQUIRED COMPONENTS filesystem)
# target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Plugins/Languages/
)

# move duktape dll to ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Core/duktape.dll ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
)
# remove duktape dll from ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Plugins/Languages/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Plugins/Languages/duktape.dll
)