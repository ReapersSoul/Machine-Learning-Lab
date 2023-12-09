git clone https://github.com/Microsoft/vcpkg.git
sudo ./vcpkg/bootstrap-vcpkg.bat
sudo ./vcpkg/vcpkg install nlohmann-json:x64-windows --recurse
sudo ./vcpkg/vcpkg install sqlite3:x64-windows --recurse
sudo ./vcpkg/vcpkg install opencl:x64-windows --recurse
sudo ./vcpkg/vcpkg install opengl:x64-windows --recurse
sudo ./vcpkg/vcpkg install glew:x64-windows --recurse
sudo ./vcpkg/vcpkg install glfw3:x64-windows --recurse
sudo ./vcpkg/vcpkg install glm:x64-windows --recurse
sudo ./vcpkg/vcpkg install imgui[docking-experimental,opengl3-binding,glfw-binding]:x64-windows --recurse
sudo ./vcpkg/vcpkg install imgui-node-editor:x64-windows --recurse
sudo ./vcpkg/vcpkg install lua:x64-windows --recurse --recurse
sudo ./vcpkg/vcpkg install chaiscript:x64-windows --recurse
sudo ./vcpkg/vcpkg install duktape:x64-windows --recurse
sudo ./vcpkg/vcpkg install python3:x64-windows --recurse
sudo ./vcpkg/vcpkg install stb:x64-windows --recurse
sudo ./vcpkg/vcpkg install boost:x64-windows --recurse
sudo ./vcpkg/vcpkg install boost-dll:x64-windows --recurse
sudo ./vcpkg/vcpkg integrate install
./Build.bat