project(LossEngine)

add_library(${PROJECT_NAME} SHARED "LossEngine.cpp")

# Set standard to C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# OpenCl
find_package(OpenCL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCL_LIBRARIES})

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)

set(ESCAPE_DEPTH /../../../../)
set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}${ESCAPE_DEPTH}Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)

target_include_directories(${PROJECT_NAME} PUBLIC ${ENGINE_IN_PATH}LossEngine/LossEngineInterface/)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
	
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Machine-Learning-Lab/Core/
)

