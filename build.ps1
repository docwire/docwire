Param (
    [string]$BuildType = 'Release'
)

choco install archiver -y
choco install doxygen.install -y
choco install graphviz -y

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
mkdir downloads
Invoke-WebRequest -Uri https://github.com/microsoft/vcpkg/files/7075269/nasm-2.15.05-win32.zip -OutFile downloads\nasm-2.15.05-win32.zip
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
vcpkg\vcpkg install podofo:$VCPKG_TRIPLET
vcpkg\vcpkg install pthreads:$VCPKG_TRIPLET

$vcpkg_path="$PWD\vcpkg"
$vcpkg_toolchain="$vcpkg_path\scripts\buildsystems\vcpkg.cmake"
$vcpkg_prefix="$vcpkg_path\installed\$VCPKG_TRIPLET"

$deps_prefix="$PWD\deps"
mkdir $deps_prefix

git clone https://github.com/docwire/htmlcxx.git
cd htmlcxx
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_MSVC_RUNTIME_LIBRARY='MultiThreaded$<$<CONFIG:Debug>:Debug>DLL' -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config $BuildType
cmake --build . --config $BuildType --target install
cd ..\..

Invoke-WebRequest -Uri http://silvercoders.com/download/3rdparty/libcharsetdetect-master.tar.bz2 -OutFile libcharsetdetect-master.tar.bz2
arc unarchive libcharsetdetect-master.tar.bz2
cd libcharsetdetect-master
Remove-Item -Path 'CMakeLists.txt'
Write-Output 'cmake_minimum_required(VERSION 3.16)' `
      'project(charsetdetect)' `
      'set(CMAKE_CXX_STANDARD 17)' `
      'set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")' `
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
cmake --build . --config $BuildType
cmake --build . --config $BuildType --target install
cd ..
cd ..

git clone https://github.com/docwire/wv2.git
cd wv2
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain"  -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config $BuildType
cmake --build . --config $BuildType --target install
cd ..\..
Invoke-WebRequest -Uri http://silvercoders.com/download/3rdparty/wv2-0.2.3_patched_4-private_headers.tar.bz2 -OutFile wv2-0.2.3_patched_4-private_headers.tar.bz2
arc unarchive wv2-0.2.3_patched_4-private_headers.tar.bz2
Move-Item -Path wv2-0.2.3_patched_4-private_headers\*.h -Destination "$deps_prefix\include\wv2"

git clone https://github.com/docwire/mimetic.git
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/CMakeLists.txt -OutFile mimetic\CMakeLists.txt
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/win-vs140.cmake -OutFile mimetic\win-vs140.cmake
cd mimetic
mkdir cmake
Invoke-WebRequest -Uri https://raw.githubusercontent.com/richiware/mimetic/master/cmake/config.cmake.in -OutFile cmake\config.cmake.in
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config $BuildType
cmake --build . --config $BuildType --target install
cd ..\..

git clone https://github.com/docwire/bfio.git
cd bfio
.\synclibs.ps1
.\autogen.ps1
cd msvscpp
if ($BuildType -eq "Debug")
{
	$bfio_conf="VSDebug"
}
else
{
	$bfio_conf="Release"
}
$msbuild = vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe | select-object -first 1
& $msbuild libbfio.sln /property:Configuration=$bfio_conf /property:Platform=x64 /p:PlatformToolset=v142
cd ..
Copy-Item -Path "msvscpp\$bfio_conf\*.lib" -Destination "$deps_prefix\lib" -Recurse
Copy-Item -Path "msvscpp\$bfio_conf\*.dll" -Destination "$deps_prefix\bin" -Recurse
Copy-Item -Path 'include\*.h' -Destination "$deps_prefix\include"
Copy-Item -Path 'include\libbfio' -Destination "$deps_prefix\include" -Recurse
cd ..

vcpkg\vcpkg install libpff:$VCPKG_TRIPLET

Invoke-WebRequest -Uri http://www.winimage.com/zLibDll/unzip101e.zip -OutFile unzip101e.zip
arc unarchive unzip101e.zip
cd unzip101e
Write-Output 'cmake_minimum_required(VERSION 3.16)' `
      'project(Unzip)' `
      'set(CMAKE_CXX_STANDARD 17)' `
      'set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")' `
      'set(UNZIP_SRC ioapi.c unzip.c)'`
      'set(FLAGS -fPIC)'`
      'add_library(unzip STATIC ${UNZIP_SRC})'`
      'find_path(zlib_incdir zlib.h REQUIRED)' `
      'target_include_directories(unzip PUBLIC ${zlib_incdir})' `
      'install(FILES unzip.h ioapi.h DESTINATION include)'`
      'install(TARGETS unzip DESTINATION lib)'`
      'target_compile_options(unzip PRIVATE -fPIC)'`
      > CMakeLists.txt
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_PREFIX_PATH="$deps_prefix" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix"
cmake --build . --config $BuildType
cmake --build . --config $BuildType --target install
cd ..
cd ..
Remove-Item -Path unzip101e.zip

dir -s "$vcpkg_prefix"
dir -s "$deps_prefix"

mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_MSVC_RUNTIME_LIBRARY='MultiThreaded$<$<CONFIG:Debug>:Debug>DLL' -DCMAKE_PREFIX_PATH="$deps_prefix"
cmake --build . -j6 --config $BuildType
cmake --build . --config $BuildType --target doxygen install
cd ..

cd build
mkdir tessdata
cd tessdata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/eng.traineddata -OutFile eng.traineddata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/osd.traineddata -OutFile osd.traineddata
Invoke-WebRequest -Uri https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/pol.traineddata -OutFile pol.traineddata
cd ..
cd ..
if ($BuildType -eq "Debug")
{
	$vcpkg_bin_dir="$vcpkg_prefix/debug/bin"
	$debug_suffix="d"
	$debug_suffix2="-d"
	$boost_arch="vc142-mt-gd-x64"
}
else
{
	$vcpkg_bin_dir="$vcpkg_prefix/bin"
	$debug_suffix=""
	$debug_suffix2=""
	$boost_arch="vc142-mt-x64"
}
$LIB_PATHS=(
    "C:\Windows\System32\VCRUNTIME140_1${debug_suffix}.dll",
    "$deps_prefix/bin/htmlcxx.dll",
    "$deps_prefix/bin/wv2.dll",
    "$vcpkg_bin_dir/boost_filesystem-${boost_arch}-1_81.dll",
    "$vcpkg_bin_dir/brotlicommon.dll",
    "$vcpkg_bin_dir/brotlidec.dll",
    "$vcpkg_bin_dir/bz2${debug_suffix}.dll",
    "$vcpkg_bin_dir/freetype${debug_suffix}.dll",
    "$vcpkg_bin_dir/gif.dll",
    "$vcpkg_bin_dir/iconv-2.dll",
    "$vcpkg_bin_dir/jpeg62.dll",
    "$vcpkg_bin_dir/leptonica-1.82.0${debug_suffix}.dll",
    "$vcpkg_bin_dir/tesseract52${debug_suffix}.dll",
    "$vcpkg_bin_dir/archive.dll",
    "$vcpkg_bin_dir/zstd.dll",
    "$vcpkg_bin_dir/libcurl${debug_suffix2}.dll",
    "$vcpkg_bin_dir/libcrypto-3-x64.dll",
    "$vcpkg_bin_dir/liblzma.dll",
    "$deps_prefix/bin/libbfio.dll",
    "$vcpkg_bin_dir/libpff.dll",
    "$vcpkg_bin_dir/libpng16${debug_suffix}.dll",
    "$vcpkg_bin_dir/libxml2.dll",
    "$vcpkg_bin_dir/openjp2.dll",
    "$vcpkg_bin_dir/podofo.dll",
    "$vcpkg_bin_dir/tiff${debug_suffix}.dll",
    "$vcpkg_bin_dir/webp.dll",
    "$vcpkg_bin_dir/webpmux.dll",
    "$vcpkg_bin_dir/zlib${debug_suffix}1.dll");
if ($BuildType -eq "Debug")
{
	$LIB_PATHS += "C:\Windows\System32\MSVCP140D.dll"
	$LIB_PATHS += "C:\Windows\System32\VCRUNTIME140D.dll"
	$LIB_PATHS += "C:\Windows\System32\ucrtbased.dll"
	$LIB_PATHS += "$vcpkg_bin_dir/pthreadVC3d.dll"
}

foreach ($PATH in $LIB_PATHS){echo $PATH; Copy-Item -Path $PATH -Destination build/};

cd build
ctest -VV --debug --output-on-failure --stop-on-failure --timeout 30 --repeat until-pass:3
cd ..

Get-ChildItem -Path build\ -Recurse -Filter *.dll | Select-Object -Property Name,@{name="Hash";expression={(Get-FileHash $_.FullName).hash}} > build\SHA1checksums.sha1

$version = Get-Content build/VERSION

if ($BuildType -eq "Debug")
{
	$arch="msvc-debug"
}
else
{
	$arch="msvc"
}

mkdir doctotext-$version-$arch

Copy-Item -Path "build\*.dll" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\*.lib" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\*.pdb" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\*.exe" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\*.h" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\*.hpp" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\SHA1checksums.sha1" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\VERSION" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "ChangeLog" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\plugins" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\tessdata" -Destination "doctotext-$version-$arch" -Recurse
Copy-Item -Path "build\doc" -Destination "doctotext-$version-$arch" -Recurse

Compress-Archive -LiteralPath doctotext-$version-$arch -DestinationPath doctotext-$version-$arch.zip
Get-FileHash -Algorithm SHA1 doctotext-$version-$arch.zip > doctotext-$version-$arch.zip.sha1

Remove-Item -Path doctotext-$version-$arch -Recurse
