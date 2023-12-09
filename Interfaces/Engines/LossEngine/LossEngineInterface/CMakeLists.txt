project(LossEngineInterface)

add_library(${PROJECT_NAME} INTERFACE "LossEngineInterface.hpp")

target_link_libraries(${PROJECT_NAME} INTERFACE LossInterface)