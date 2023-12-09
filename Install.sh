sudo apt-get install gcc build-essential gdb cmake curl zip unzip tar pkg-config autoconf autoconf-archive libxmu-dev libxi-dev libgl-dev libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev
git clone https://github.com/Microsoft/vcpkg.git
chmod +x ./vcpkg/bootstrap-vcpkg.sh
sudo ./vcpkg/bootstrap-vcpkg.sh
sudo ./vcpkg/vcpkg install nlohmann-json:x64-linux --recurse
sudo ./vcpkg/vcpkg install sqlite3:x64-linux --recurse
sudo ./vcpkg/vcpkg install opencl:x64-linux --recurse
sudo ./vcpkg/vcpkg install opengl:x64-linux --recurse
sudo ./vcpkg/vcpkg install glew:x64-linux --recurse
sudo ./vcpkg/vcpkg install glfw3:x64-linux --recurse
sudo ./vcpkg/vcpkg install glm:x64-linux --recurse
sudo ./vcpkg/vcpkg install imgui[docking-experimental,opengl3-binding,glfw-binding]:x64-linux --recurse
sudo ./vcpkg/vcpkg install imgui-node-editor:x64-linux --recurse
sudo ./vcpkg/vcpkg install lua:x64-linux --recurse --recurse
sudo ./vcpkg/vcpkg install chaiscript:x64-linux --recurse
sudo ./vcpkg/vcpkg install duktape:x64-linux --recurse
sudo ./vcpkg/vcpkg install python3:x64-linux --recurse
sudo ./vcpkg/vcpkg install stb:x64-linux --recurse
sudo ./vcpkg/vcpkg install boost:x64-linux --recurse
sudo ./vcpkg/vcpkg install boost-dll:x64-linux --recurse
sudo ./vcpkg/vcpkg integrate install
chmod +x ./Build.sh
sudo ./Build.sh
