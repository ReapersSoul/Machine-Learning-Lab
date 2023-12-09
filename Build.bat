cmake -B ./Build -S ./ -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build ./Build --config Debug
mkdir ./Build/Runtime/Debug/Machine-Learning-Lab/Libs
Start ./Build/Runtime/Debug/Cleaner/Cleaner.exe