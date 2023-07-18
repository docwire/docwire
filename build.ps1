choco install archiver -y
choco install doxygen.install -y

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.01.09
.\bootstrap-vcpkg.bat
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'libtool-2.4.6-9-x86_64.pkg.tar.xz', 'libtool-2.4.6-14-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'b309799e5a9d248ef66eaf11a0bd21bf4e8b9bd5c677c627ec83fa760ce9f0b54ddf1b62cbb436e641fbbde71e3b61cb71ff541d866f8ca7717a3a0dbeb00ebf', 'ba983ed9c2996d06b0d21b8fab9505267115f2106341f130e92d6b66dad87b0f0e82707daf0b676a28966bfaa24f6c41b6eef9e1f9bf985611471024f2b0ac97' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'grep-3.0-2-x86_64.pkg.tar.xz', 'grep-1~3.0-6-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'c784d5f8a929ae251f2ffaccf7ab0b3936ae9f012041e8f074826dd6077ad0a859abba19feade1e71b3289cc640626dfe827afe91c272b38a1808f228f2fdd00', '79b4c652082db04c2ca8a46ed43a86d74c47112932802b7c463469d2b73e731003adb1daf06b08cf75dc1087f0e2cdfa6fec0e8386ada47714b4cff8a2d841e1' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'msys2-runtime-3.2.0-8-x86_64.pkg.tar.zst', 'msys2-runtime-3.2.0-15-x86_64.pkg.tar.zst' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
(Get-Content scripts\cmake\vcpkg_acquire_msys.cmake) -replace 'fdd86f4ffa6e274d6fef1676a4987971b1f2e1ec556eee947adcb4240dc562180afc4914c2bdecba284012967d3d3cf4d1a392f798a3b32a3668d6678a86e8d3', 'e054f5e1fc87add7fc3f5cbafd0ff5f3f77a30385073c17d0a8882feb3319a4cba355ccb1cb67ccb9fa704b514c3d05f54c45199011f604c69bb70e52fa33455' | Set-Content scripts\cmake\vcpkg_acquire_msys.cmake
cd ..

$VCPKG_TRIPLET="x64-windows"

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

$vcpkg_path="$PWD\vcpkg"
$vcpkg_toolchain="$vcpkg_path\scripts\buildsystems\vcpkg.cmake"
$vcpkg_prefix="$vcpkg_path\installed\$VCPKG_TRIPLET"

$deps_prefix="$PWD\deps"
mkdir $deps_prefix

git clone https://github.com/docwire/htmlcxx.git
cd htmlcxx
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
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
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..
cd ..

git clone https://github.com/docwire/wv2.git
cd wv2
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain"  -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..\..
Invoke-WebRequest -Uri http://silvercoders.com/download/3rdparty/wv2-0.2.3_patched_4-private_headers.tar.bz2 -OutFile wv2-0.2.3_patched_4-private_headers.tar.bz2
arc unarchive wv2-0.2.3_patched_4-private_headers.tar.bz2
Move-Item -Path wv2-0.2.3_patched_4-private_headers\*.h -Destination "$deps_prefix\include\wv2"

git clone https://github.com/docwire/mimetic.git
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/CMakeLists.txt -OutFile mimetic\CMakeLists.txt
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/win-vs140.cmake -OutFile mimetic\win-vs140.cmake
(Get-Content mimetic\win-vs140.cmake) -replace 'MD', 'MT' | Set-Content mimetic\win-vs140.cmake
cd mimetic
mkdir cmake
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/cmake/config.cmake.in -OutFile cmake\config.cmake.in
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
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
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_CXX_FLAGS="-I$vcpkg_prefix/include" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config Release
cmake --build . --config Release --target install
cd ..
cd ..

mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_MSVC_RUNTIME_LIBRARY='MultiThreaded$<$<CONFIG:Debug>:Debug>' -DCMAKE_PREFIX_PATH="$deps_prefix"
cmake --build . -j6 --config Release
cmake --build . --config Release --target doxygen install
cd ..

cd build
mkdir tessdata
cd tessdata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/eng.traineddata -OutFile eng.traineddata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/osd.traineddata -OutFile osd.traineddata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/pol.traineddata -OutFile pol.traineddata
cd ..
cd ..
$LIB_PATHS=(
    "$deps_prefix/bin/htmlcxx.dll",
    "$deps_prefix/bin/wv2.dll",
    "$vcpkg_prefix/bin/boost_filesystem-vc143-mt-x64-1_79.dll",
    "$vcpkg_prefix/bin/brotlicommon.dll",
    "$vcpkg_prefix/bin/brotlidec.dll",
    "$vcpkg_prefix/bin/bz2.dll",
    "$vcpkg_prefix/bin/freetype.dll",
    "$vcpkg_prefix/bin/gif.dll",
    "$vcpkg_prefix/bin/iconv-2.dll",
    "$vcpkg_prefix/bin/jpeg62.dll",
    "$vcpkg_prefix/bin/leptonica-1.82.0.dll",
    "$vcpkg_prefix/bin/libcrypto-3-x64.dll",
    "$vcpkg_prefix/bin/liblzma.dll",
    "$vcpkg_prefix/bin/libpff.dll",
    "$vcpkg_prefix/bin/libpng16.dll",
    "$vcpkg_prefix/bin/libxml2.dll",
    "$vcpkg_prefix/bin/openjp2.dll",
    "$vcpkg_prefix/bin/podofo.dll",
    "$vcpkg_prefix/bin/pthreadVC3.dll",
    "$vcpkg_prefix/bin/tiff.dll",
    "$vcpkg_prefix/bin/webp.dll",
    "$vcpkg_prefix/bin/webpmux.dll",
    "$vcpkg_prefix/bin/zlib1.dll");
foreach ($PATH in $LIB_PATHS){echo $PATH; Copy-Item -Path $PATH -Destination build/};

cd build
ctest -V
cd ..

Get-ChildItem -Path build\ -Recurse -Filter *.dll | Select-Object -Property Name,@{name="Hash";expression={(Get-FileHash $_.FullName).hash}} > build\SHA1checksums.sha1

$version = Get-Content build/VERSION

mkdir doctotext-$version-msvc

Copy-Item -Path "build\*.dll" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\*.lib" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\*.pdb" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\*.exe" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\*.h" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\*.hpp" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\SHA1checksums.sha1" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\VERSION" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "ChangeLog" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\plugins" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\tessdata" -Destination "doctotext-$version-msvc" -Recurse
Copy-Item -Path "build\doc" -Destination "doctotext-$version-msvc" -Recurse

Compress-Archive -LiteralPath doctotext-$version-msvc -DestinationPath doctotext-$version-msvc.zip
Get-FileHash -Algorithm SHA1 doctotext-$version-msvc.zip > doctotext-$version-msvc.zip.sha1

Remove-Item -Path doctotext-$version-msvc -Recurse
