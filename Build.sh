cmake -B ./Build -S ./ -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build ./Build
if [ ! -d "./Runtime" ]; then
  mkdir Runtime
fi

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/
fi
cp ./Build/Machine-Learning-Lab/Machine-Learning-Lab ./Build/Runtime/Debug/Machine-Learning-Lab/Machine-Learning-Lab

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/
fi
find ./Build/Implementations/Engines/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Core/ \; 

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/Activations/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Activations/
fi
find ./Build/Implementations/Activations/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Activations/ \;

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/Losses/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Losses/
fi
find ./Build/Implementations/Losses/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Losses/ \;

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/
fi

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Sorters/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Sorters/
fi
find ./Build/Implementations/Sorters/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Sorters/ \;

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Nodes/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Nodes/
fi
find ./Build/Implementations/Nodes/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Core/Graph/Nodes/ \;

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Plugins/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Plugins/
fi

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Plugins/Languages/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Plugins/Languages/
fi
find ./Build/Implementations/Languages/ -type f \( -name '*.so' \) -exec cp {} ./Build/Runtime/Debug/Machine-Learning-Lab/Plugins/Languages/ \;

if [ ! -d "./Build/Runtime/Debug/Machine-Learning-Lab/Libs/" ]; then
  mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Libs/
fi
