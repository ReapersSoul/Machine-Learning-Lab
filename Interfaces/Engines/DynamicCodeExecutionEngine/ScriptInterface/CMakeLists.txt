project(ScriptInterface)

add_library(${PROJECT_NAME} INTERFACE "ScriptInterface.h")

set(INTERFACE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Interfaces/)
set(IMPLIMENTATIONS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Implimentations/)
set(ENGINE_IN_PATH ${INTERFACE_PATH}Engines/)
set(ENGINE_IM_PATH ${IMPLIMENTATIONS_PATH}Engines/)
get_filename_component(PINTERFACE ${INTERFACE_PATH} ABSOLUTE)
get_filename_component(PIMPLIMENTATION ${IMPLIMENTATIONS_PATH} ABSOLUTE)
get_filename_component(PEINTERFACE ${ENGINE_IN_PATH} ABSOLUTE)
get_filename_component(PEIMPLIMENTATION ${ENGINE_IM_PATH} ABSOLUTE)

target_include_directories(${PROJECT_NAME} INTERFACE ${PEINTERFACE}/DynamicCodeExecutionEngine/Languages/LanguageInterface/)
target_link_libraries(${PROJECT_NAME} INTERFACE ${CMAKE_BINARY_DIR}/Interfaces/LanguageInterface/)