project(Lua_Language)

add_library(${PROJECT_NAME} SHARED "Lua_Language.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Lua
find_package(Lua REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LUA_LIBRARIES})

# glew
find_package(glew REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GLEW_LIBRARIES})

# glfw
find_package(glfw3 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# glm
find_package(glm REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# imgui
find_package(imgui REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

# OpenCl
# find_package(OpenCL REQUIRED)
# target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)

# nlohmann_json
# find_package(nlohmann_json REQUIRED)
# target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# get parent path
set(ESCAPE_DEPTH /../../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)

# DCEE
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/Plugins/PluginInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/LibraryInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/LanguageInterface/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/DynamicCodeExecutionEngine/ScriptInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC DynamicCodeExecutionEngineInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC LanguageInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC PluginInterface)
target_link_libraries(${PROJECT_NAME} PUBLIC ScriptInterface)

# UIE
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/UIEngine/UIEngineInterface/)

# GraphEngine
target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/GraphEngineInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphEngineInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/EdgeInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC EdgeInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/GraphInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC GraphInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/NodeIOInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC NodeIOInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/NodeInterface/)
target_link_libraries(${PROJECT_NAME} PUBLIC NodeInterface)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/GraphEngine/AttributeInterface/)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Plugins/Languages/
)