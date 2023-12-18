apt-get install gcc build-essential gdb cmake curl zip unzip tar pkg-config autoconf autoconf-archive libxmu-dev libxi-dev libgl-dev libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev

#sudo git clone https://github.com/Microsoft/vcpkg.git /vcpkg
#sudo chmod +x /vcpkg/bootstrap-vcpkg.sh
#export VCPKG_FORCE_SYSTEM_BINARIES=1
#export PATH=/vcpkg:$PATH
#sudo ./vcpkg/bootstrap-vcpkg.sh

vcpkg install nlohmann-json:x64-linux --recurse
vcpkg install sqlite3:x64-linux --recurse
vcpkg install opencl:x64-linux --recurse
vcpkg install opengl:x64-linux --recurse
vcpkg install glew:x64-linux --recurse
vcpkg install glfw3:x64-linux --recurse
vcpkg install glm:x64-linux --recurse
vcpkg install imgui[docking-experimental,opengl3-binding,glfw-binding]:x64-linux --recurse
vcpkg install imgui-node-editor:x64-linux --recurse
vcpkg install lua:x64-linux --recurse
vcpkg install chaiscript:x64-linux --recurse
vcpkg install duktape:x64-linux --recurse
vcpkg install python3:x64-linux --recurse
vcpkg install stb:x64-linux --recurse
vcpkg install boost:x64-linux --recurse
vcpkg install boost-dll:x64-linux --recurse
vcpkg integrate install

#sudo chmod +x ./Build.sh
#sudo ./Build.sh
