apt-get install gcc build-essential gdb cmake curl zip unzip tar pkg-config autoconf autoconf-archive libxmu-dev libxi-dev libgl-dev libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev

#sudo git clone https://github.com/Microsoft/vcpkg.git /vcpkg
#sudo chmod +x /vcpkg/bootstrap-vcpkg.sh
#export VCPKG_FORCE_SYSTEM_BINARIES=1
#export PATH=/vcpkg:$PATH
#sudo ./vcpkg/bootstrap-vcpkg.sh

vcpkg install
vcpkg integrate install

#sudo chmod +x ./Build.sh
#sudo ./Build.sh
