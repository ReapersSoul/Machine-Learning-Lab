project(LossEngineInterface)

add_library(${PROJECT_NAME} INTERFACE "LossEngineInterface.h")

target_link_libraries(${PROJECT_NAME} INTERFACE LossInterface)