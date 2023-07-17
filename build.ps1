choco install archiver -y
choco install doxygen -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.01.09
.\bootstrap-vcpkg.bat
cd ..

VCPKG_TRIPLET=x64-windows

vcpkg\vcpkg install libiconv:$VCPKG_TRIPLET
vcpkg\vcpkg install zlib:$VCPKG_TRIPLET
vcpkg\vcpkg install freetype:$VCPKG_TRIPLET
vcpkg\vcpkg install libxml2:$VCPKG_TRIPLET
vcpkg\vcpkg install leptonica:$VCPKG_TRIPLET
vcpkg\vcpkg install tesseract:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-filesystem:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-system:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-signals2:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-config:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-dll:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-assert:$VCPKG_TRIPLET
vcpkg\vcpkg install boost-smart-ptr:$VCPKG_TRIPLET

vcpkg_path="$PWD\vcpkg"
vcpkg_toolchain="$vcpkg_path\scripts\buildsystems\vcpkg.cmake"
vcpkg_prefix="$vcpkg_path\installed\$VCPKG_TRIPLET"

deps_prefix="$PWD\deps"
mkdir $deps_prefix

git clone https://github.com/docwire/htmlcxx.git
cd htmlcxx
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..\..

Invoke-WebRequest -Uri http://silvercoders.com/download/3rdparty/libcharsetdetect-master.tar.bz2 -OutFile libcharsetdetect-master.tar.bz2
arc unarchive libcharsetdetect-master.tar.bz2
cd libcharsetdetect-master
Remove-Item -Path 'CMakeLists.txt'
Write-Output 'cmake_minimum_required(VERSION 3.16)' `
      'project(charsetdetect)' `
      'set(CMAKE_CXX_STANDARD 17)' `
      'set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")' `
      'set(FLAGS -fPIC)'`
      'include_directories(. nspr-emu mozilla/extensions/universalchardet/src/base/)'`
      'file(GLOB charsetdetect_lib_src mozilla/extensions/universalchardet/src/base/*.cpp)'`
      'set(charsetdetect_lib_src ${charsetdetect_lib_src} charsetdetect.cpp)'`
      'add_library(charsetdetect STATIC ${charsetdetect_lib_src})'`
      'set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)' `
      'install(TARGETS charsetdetect DESTINATION lib)'`
      'install(FILES charsetdetect.h DESTINATION include)'`
      > CMakeLists.txt
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..
cd ..

git clone https://github.com/docwire/wv2.git
cd wv2
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain"  -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..\..
Invoke-WebRequest -Uri http://silvercoders.com/download/3rdparty/wv2-0.2.3_patched_4-private_headers.tar.bz2 -OutFile wv2-0.2.3_patched_4-private_headers.tar.bz2
arc unarchive wv2-0.2.3_patched_4-private_headers.tar.bz2
Move-Item -Path wv2-0.2.3_patched_4-private_headers\*.h -Destination '$deps_dir\include\wv2

git clone https://github.com/docwire/mimetic.git
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/CMakeLists.txt -OutFile mimetic\CMakeLists.txt
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/win-vs140.cmake -OutFile mimetic\win-vs140.cmake
(Get-Content mimetic\win-vs140.cmake) -replace 'MD', 'MT' | Set-Content mimetic\win-vs140.cmake
cd mimetic
mkdir cmake
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/cmake/config.cmake.in -OutFile config.cmake.in
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_dir"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..\..

vcpkg\vcpkg install libpff:$VCPKG_TRIPLET

Invoke-WebRequest -Uri http://www.winimage.com/zLibDll/unzip101e.zip -OutFile unzip101e.zip
arc unarchive unzip101e.zip
cd unzip101e
Write-Output 'cmake_minimum_required(VERSION 3.16)' `
      'project(Unzip)' `
      'set(CMAKE_CXX_STANDARD 17)' `
      'set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")' `
      'set(UNZIP_SRC ioapi.c unzip.c)'`
      'set(FLAGS -fPIC)'`
      'add_library(unzip STATIC ${UNZIP_SRC})'`
      'install(FILES unzip.h ioapi.h DESTINATION include)'`
      'install(TARGETS unzip DESTINATION lib)'`
      'target_compile_options(unzip PRIVATE -fPIC)'`
      > CMakeLists.txt
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_PREFIX_PATH="$vcpkg_prefix" -DCMAKE_INSTALL_PREFIX:PATH="$deps_dir"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..
cd ..

mkdir build
cd build
cmake .. -G 'Visual Studio 17 2022' -A x64 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_MSVC_RUNTIME_LIBRARY='MultiThreaded$<$<CONFIG:Debug>:Debug>' -DCMAKE_PREFIX_PATH="$deps_prefix"
cmake --build . -j6 --config Release
cmake --build . --config Release --target doxygen install
cd ..

cd build
ctest -V
cd ..
