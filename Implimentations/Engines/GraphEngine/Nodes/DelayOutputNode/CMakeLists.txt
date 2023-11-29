set(PROJECT_NAME DelayOutputNode)

################################################################################
# Source groups
################################################################################
set(Source_Files
    "DelayOutputNode.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Implimentations/Engines/GraphEngine/Nodes")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Includes for CMake from *.props
################################################################################
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/DynamicCodeExecutionEngine/DynamicCodeExecutionEngineInterface/DynamicCodeExecutionEngineInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/GraphEngine/Nodes/NodeInterface/NodeInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/DynamicCodeExecutionEngine/Languages/LanguageInterface/LanguageInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/GraphEngine/GraphEngineInterface/GraphEngineInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/GraphEngine/Attributes/AttributeInterface/AttributeInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/GraphEngine/EdgeInterface/EdgeInterface/EdgeInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/GraphEngine/GraphInterface/GraphInterface/GraphInterface.vcxitems")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "../../../../../Interfaces/Engines/SerializationEngine/SerializableInterface/SerializableInterface/SerializableInterface.vcxitems")

set(ROOT_NAMESPACE DelayOutputNode)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Machine Learning Labs/Core/Graph/Nodes"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Machine Learning Labs/Core/Graph/Nodes"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "DELAYOUTPUTNODE_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "DELAYOUTPUTNODE_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

