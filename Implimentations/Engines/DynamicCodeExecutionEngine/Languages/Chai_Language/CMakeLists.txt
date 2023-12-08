project(Chai_Language)

add_library(${PROJECT_NAME} SHARED "Chai_Language.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# get parent path
set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}/ActivationEngine/Activations/ActivationInterface/)

# Chai
find_package(ChaiScript REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${ChaiScript_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${ChaiScript_LIBRARIES})

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
# find_package(nlohmann_json REQUIRED)
# target_include_directories(${PROJECT_NAME} PUBLIC ${nlohmann_json_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Core/Activations/
)

# move chai dll to ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Core/chaiscript_stdlib.dll ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Libs/
)
# remove chai dll from ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Core/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Machine-Learning-Labs-CrossPlatform-V2/Core/chaiscript_stdlib.dll
)