git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.bat
./vcpkg/vcpkg install nlohmann-json:x64-windows
./vcpkg/vcpkg install sqlite3:x64-windows
./vcpkg/vcpkg install opencl:x64-windows
./vcpkg/vcpkg install lua:x64-windows
./vcpkg/vcpkg install chaiscript:x64-windows
./vcpkg/vcpkg install duktape:x64-windows
./vcpkg/vcpkg install python3:x64-windows
./vcpkg/vcpkg install jdk:x64-windows
./vcpkg/vcpkg install boost:x64-windows

cmake -B ./Build -S ./ -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake