# Install script for directory: C:/Users/Trevor/Desktop/Machine-Learning-Lab

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/install/x64-debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/LanguageInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/NetworkObjectInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/ScriptInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/LibraryInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/PluginInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/SerializableInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/EngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/LedgerInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/ClientInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/ServerInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/DynamicCodeExecutionEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/UIEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/NetworkEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/LossInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/ActivationInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/GraphInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/NodeIOInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/EdgeInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/AttributeInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/NodeInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/LossEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/ActivationEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/GraphEngineInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Interfaces/SorterInterface/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/DynamicCodeExecutionEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/UIEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/GraphEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/ActivationEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/LossEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/DefaultSorter/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/NetworkEngine/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Activations/LeakyReLUActivation/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Activations/ReLUActivation/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Activations/SigmoidActivation/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Activations/ScriptActivation/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Losses/MSELoss/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Losses/ScriptLoss/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Languages/Lua_Language/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ViewNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/UniqueNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/SubGraphNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/SelfAttentionLayerNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ScriptNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/RecurrentNetworkNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/RecurrentLayerNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/PositionalEncodingNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/OneHotNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/NormalizeNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/MathNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/LayerNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ImageNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ImageLossNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/FullyConnectedNetworkNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/FlattenNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/FileVisualizationNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/DictionaryNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/DelayOutputNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/DataToTensorNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/DataToMatrixNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/DatabaseInputNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/Convolution3DNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/Convolution2DNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/Convolution2DNetworkNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ConversionNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ControllerNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ValueNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/LossNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/LossDataSetNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/InputDataSetNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/ColumnNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Implimentations/Nodes/AtIndexNode/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Machine-Learning-Labs-CrossPlatform-V2/cmake_install.cmake")
  include("C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/Cleaner/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Trevor/Desktop/Machine-Learning-Lab/out/build/x64-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")