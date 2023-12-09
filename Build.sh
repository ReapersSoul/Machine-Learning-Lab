cmake -B ./Build -S ./ -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
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
cp -r ./Build/Implementations/Engines/*.so ./Runtime/Machine-Learning-Lab/Core/

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Activations/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Activations/
fi
cp -r ./Build/Implementations/Activations/*.so ./Runtime/Machine-Learning-Lab/Core/Activations/

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Losses/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Losses/
fi
cp -r ./Build/Implementations/Losses/*.so ./Runtime/Machine-Learning-Lab/Core/Losses/

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Graph/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Graph/
fi

if [ ! -d "./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/
fi
cp -r ./Build/Implementations/Nodes/*.so ./Runtime/Machine-Learning-Lab/Core/Graph/Nodes/

if [ ! -d "./Runtime/Machine-Learning-Lab/Plugins/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Plugins/
fi

if [ ! -d "./Runtime/Machine-Learning-Lab/Plugins/Languages/" ]; then
  mkdir ./Runtime/Machine-Learning-Lab/Plugins/Languages/
fi
cp -r ./Build/Implementations/Languages/*.so ./Runtime/Machine-Learning-Lab/Plugins/Languages/