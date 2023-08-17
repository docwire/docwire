set -e

if [[ "$OSTYPE" == "darwin"* ]]; then
	brew update
	brew install md5sha1sum automake autogen doxygen
elif [[ "$OSTYPE" == "linux"* ]]; then
	if [[ "$GITHUB_ACTIONS" == "true" ]]; then
		sudo apt-get install -y autopoint
		sudo apt-get install -y doxygen
	fi
elif [[ "$OSTYPE" != "msys"* ]]; then
	echo "Unknown OS type." >&2
	exit 1
fi

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.01.09
./bootstrap-vcpkg.sh
if [[ "$OSTYPE" == "msys"* ]]; then
	sed -i s/libtool-2.4.6-9-x86_64.pkg.tar.xz/libtool-2.4.6-14-x86_64.pkg.tar.zst/ scripts/cmake/vcpkg_acquire_msys.cmake
	sed -i s/b309799e5a9d248ef66eaf11a0bd21bf4e8b9bd5c677c627ec83fa760ce9f0b54ddf1b62cbb436e641fbbde71e3b61cb71ff541d866f8ca7717a3a0dbeb00ebf/ba983ed9c2996d06b0d21b8fab9505267115f2106341f130e92d6b66dad87b0f0e82707daf0b676a28966bfaa24f6c41b6eef9e1f9bf985611471024f2b0ac97/ scripts/cmake/vcpkg_acquire_msys.cmake
	sed -i s/grep-3.0-2-x86_64.pkg.tar.xz/grep-1~3.0-6-x86_64.pkg.tar.zst/ scripts/cmake/vcpkg_acquire_msys.cmake
	sed -i s/c784d5f8a929ae251f2ffaccf7ab0b3936ae9f012041e8f074826dd6077ad0a859abba19feade1e71b3289cc640626dfe827afe91c272b38a1808f228f2fdd00/79b4c652082db04c2ca8a46ed43a86d74c47112932802b7c463469d2b73e731003adb1daf06b08cf75dc1087f0e2cdfa6fec0e8386ada47714b4cff8a2d841e1/ scripts/cmake/vcpkg_acquire_msys.cmake
	sed -i s/msys2-runtime-3.2.0-8-x86_64.pkg.tar.zst/msys2-runtime-3.2.0-15-x86_64.pkg.tar.zst/ scripts/cmake/vcpkg_acquire_msys.cmake
	sed -i s/fdd86f4ffa6e274d6fef1676a4987971b1f2e1ec556eee947adcb4240dc562180afc4914c2bdecba284012967d3d3cf4d1a392f798a3b32a3668d6678a86e8d3/e054f5e1fc87add7fc3f5cbafd0ff5f3f77a30385073c17d0a8882feb3319a4cba355ccb1cb67ccb9fa704b514c3d05f54c45199011f604c69bb70e52fa33455/ scripts/cmake/vcpkg_acquire_msys.cmake
fi
cd ..

if [[ "$OSTYPE" == "darwin"* ]]; then
	VCPKG_TRIPLET=x64-osx
elif [[ "$OSTYPE" == "msys"* ]]; then
	VCPKG_TRIPLET=x64-mingw-static
else
	VCPKG_TRIPLET=x64-linux
fi

./vcpkg/vcpkg install libiconv:$VCPKG_TRIPLET
./vcpkg/vcpkg install zlib:$VCPKG_TRIPLET
./vcpkg/vcpkg install freetype:$VCPKG_TRIPLET
./vcpkg/vcpkg install libxml2:$VCPKG_TRIPLET
./vcpkg/vcpkg install leptonica:$VCPKG_TRIPLET
./vcpkg/vcpkg install tesseract:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-filesystem:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-system:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-signals2:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-config:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-dll:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-assert:$VCPKG_TRIPLET
./vcpkg/vcpkg install boost-smart-ptr:$VCPKG_TRIPLET
./vcpkg/vcpkg install mailio:$VCPKG_TRIPLET
./vcpkg/vcpkg install gtest:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports install libcharsetdetect:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports install unzip:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports install tessdata-fast:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports install cmap-resources:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports install mapping-resources-pdf:$VCPKG_TRIPLET

if [[ "$OSTYPE" == "msys"* ]]; then
	VCPKG_TRIPLET_DYNAMIC=x64-mingw-dynamic
else
	VCPKG_TRIPLET_DYNAMIC=$VCPKG_TRIPLET-dynamic
fi
mkdir -p custom-triplets
cp ./vcpkg/triplets/community/$VCPKG_TRIPLET_DYNAMIC.cmake custom-triplets/$VCPKG_TRIPLET.cmake
./vcpkg/vcpkg install podofo:$VCPKG_TRIPLET --overlay-triplets=custom-triplets
./vcpkg/vcpkg --overlay-ports=./ports --overlay-triplets=custom-triplets install htmlcxx:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports --overlay-triplets=custom-triplets install wv2:$VCPKG_TRIPLET || cat ./vcpkg/buildtrees/wv2/install-x64-osx-dbg-out.log
./vcpkg/vcpkg --overlay-ports=./ports --overlay-triplets=custom-triplets install libbfio:$VCPKG_TRIPLET
./vcpkg/vcpkg --overlay-ports=./ports --overlay-triplets=custom-triplets install libpff:$VCPKG_TRIPLET
rm -rf custom-triplets

vcpkg_path="$PWD/vcpkg"
vcpkg_toolchain="$vcpkg_path/scripts/buildsystems/vcpkg.cmake"
vcpkg_prefix="$vcpkg_path/installed/$VCPKG_TRIPLET"

mkdir -p build
cd build
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" ..
cmake --build .
cmake --build . --target doxygen install
cd ..

cd build
mkdir -p tessdata
cd tessdata
cp $vcpkg_prefix/share/tessdata-fast/eng.traineddata .
cp $vcpkg_prefix/share/tessdata-fast/osd.traineddata .
cp $vcpkg_prefix/share/tessdata-fast/pol.traineddata .
cd ..
if [[ "$OSTYPE" == "darwin"* ]]; then
	cp $vcpkg_prefix/lib/libwv2.4.dylib .
	cp $vcpkg_prefix/lib/libpodofo.0.9.8.dylib .
	cp $vcpkg_prefix/lib/libhtmlcxx.dylib .
	cp $vcpkg_prefix/lib/libbfio.1.dylib .
	cp $vcpkg_prefix/lib/libpff.1.dylib .
else
	cp $vcpkg_prefix/lib/libwv2.so.4 .
	cp $vcpkg_prefix/lib/libpodofo.so.0.9.8 .
	cp $vcpkg_prefix/lib/libhtmlcxx.so .
	cp $vcpkg_prefix/lib/libbfio.so.1 .
	cp $vcpkg_prefix/lib/libpff.so.1 .
fi
mkdir -p resources
cd resources
cp $vcpkg_prefix/share/cmap-resources/*/CMap/* .
cp $vcpkg_prefix/share/mapping-resources-pdf/pdf2unicode/* .
cd ..
cd ..

cd build/tests
if [[ "$OSTYPE" == "darwin"* ]]; then
	DYLD_FALLBACK_LIBRARY_PATH=.. ctest -j4 -V
else
	LD_LIBRARY_PATH=.. ctest -j4 -V
fi
cd ../..

build_type=$1
if [ "$build_type" = "--release" ]; then
	rm -rf build/CMakeFiles build/src build/tests/ build/examples/CMakeFiles build/doc/CMakeFiles
	rm build/Makefile build/CMakeCache.txt build/cmake_install.cmake build/install_manifest.txt build/examples/cmake_install.cmake build/examples/Makefile build/doc/Makefile build/doc/cmake_install.cmake build/doc/Doxyfile.doxygen
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
	LIB_EXTENSION=.dylib
else
	LIB_EXTENSION=.so
fi
cd build/
for i in *.$LIB_EXTENSION*; do
    [ -f "$i" ] || break
    sha1sum $i >> SHA1checksums.sha1
done
cd ..

version=`cat build/VERSION`

if [[ "$OSTYPE" == "darwin"* ]]; then
	arch=osx
elif [[ "$OSTYPE" == "msys"* ]]; then
	arch=x64_mingw
else
	arch=x86_64_linux
fi
tar -cjvf doctotext-$version-$arch.tar.bz2 build
sha1sum doctotext-$version-$arch.tar.bz2 > doctotext-$version-$arch.tar.bz2.sha1
