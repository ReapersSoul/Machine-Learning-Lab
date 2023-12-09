cmake -B ./Build -S ./ -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build ./Build
if [ ! -d "./Runtime" ]; then
  mkdir Runtime
fi

if [ ! -d "./Runtime/Machine-Learning-Lab/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/
fi
cp ./Build/Machine-Learning-Lab/Machine-Learning-Lab ./Runtime/Machine-Learning-Lab/Machine-Learning-Lab

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/
fi
find ./Build/Implementations/Engines/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Core/ \; 

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Activations/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Activations/
fi
find ./Build/Implementations/Activations/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Core/Activations/ \;

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Losses/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Losses/
fi
find ./Build/Implementations/Losses/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Core/Losses/ \;

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Graph/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Graph/
fi

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Graph/Sorters/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Graph/Sorters/
fi
find ./Build/Implementations/Sorters/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Core/Graph/Sorters/ \;

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/
fi
find ./Build/Implementations/Nodes/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/ \;

if [ ! -d "./Runtime/Machine-Learning-Lab/Plugins/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Plugins/
fi

if [ ! -d "./Runtime/Machine-Learning-Lab/Plugins/Languages/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Plugins/Languages/
fi
find ./Build/Implementations/Languages/ -type f \( -name '*.so' \) -exec cp {} ./Runtime/Machine-Learning-Lab/Plugins/Languages/ \;

if [ ! -d "./Runtime/Machine-Learning-Lab/Libs/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Libs/
fi
