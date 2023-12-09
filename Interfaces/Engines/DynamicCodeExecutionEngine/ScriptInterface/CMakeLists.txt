project(ScriptInterface)

add_library(${PROJECT_NAME} INTERFACE "ScriptInterface.hpp")

set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Interfaces/)
set(IMPLEMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Implementation/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLEMENTATIONS_PATH}Engines/)
get_filename_component(PINTERFACE ${INTERFACE_PATH} ABSOLUTE)
get_filename_component(PIMPLIMENTATION ${IMPLEMENTATIONS_PATH} ABSOLUTE)
get_filename_component(PEINTERFACE ${ENGINE_IN_PATH} ABSOLUTE)
get_filename_component(PEIMPLIMENTATION ${ENGINE_IM_PATH} ABSOLUTE)