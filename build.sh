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
	sed -i s/fdd86f4ffa6e274d6fef1676a4987971b1f2e1ec556eee947adcb4240dc562180afc4914c2bdecba284012967d3d3cf4d1a392f798a3b32a3668d6678a86e8d3/e054f5e1fc87add7fc3f5cbafd0ff5f3f77a30385073c17d0a8882feb3319a4cba355ccb1cb67ccb9fa704b514c3d05f54c45199011f604c69bb70e52fa33455/ scripts\cmake\vcpkg_acquire_msys.cmake
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

if [[ "$OSTYPE" == "msys"* ]]; then
	VCPKG_TRIPLET_DYNAMIC=x64-mingw-dynamic
else
	VCPKG_TRIPLET_DYNAMIC=$VCPKG_TRIPLET-dynamic
fi
mkdir -p custom-triplets
cp ./vcpkg/triplets/community/$VCPKG_TRIPLET_DYNAMIC.cmake custom-triplets/$VCPKG_TRIPLET.cmake
./vcpkg/vcpkg install podofo:$VCPKG_TRIPLET --overlay-triplets=custom-triplets
rm -rf custom-triplets

vcpkg_path="$PWD/vcpkg"
vcpkg_toolchain="$vcpkg_path/scripts/buildsystems/vcpkg.cmake"
vcpkg_prefix="$vcpkg_path/installed/$VCPKG_TRIPLET"

deps_prefix="$PWD/deps"
mkdir -p $deps_prefix

wget -nc https://sourceforge.net/projects/htmlcxx/files/v0.87/htmlcxx-0.87.tar.gz
echo "ac7b56357d6867f649e0f1f699d9a4f0f03a6e80  htmlcxx-0.87.tar.gz" | shasum -c
tar -xzvf htmlcxx-0.87.tar.gz
cd htmlcxx-0.87
if [[ "$OSTYPE" == "darwin"* ]]; then
	./configure CXXFLAGS=-std=c++17 LDFLAGS="-L$vcpkg_prefix/lib" LIBS="-liconv" CPPFLAGS="-I$vcpkg_prefix/include" --prefix="$deps_prefix"
else
	./configure CPPFLAGS=-std=c++17 LDFLAGS="-Wl,-no-undefined" --prefix="$deps_prefix" 
fi
sed -i.bak -e "s/\(allow_undefined=\)yes/\1no/" libtool
sed -i -r -e 's/css\/libcss_parser_pp.la \\//' Makefile
sed -i -r -e 's/css\/libcss_parser.la//' Makefile
sed -i.bak -r -e 's/-DDEFAULT_CSS=\"\\\"\$\{datarootdir\}\/htmlcxx\/css\/default.css\\\"\"//' Makefile
sed -i -r -e 's/css\/libcss_parser_pp.la//' Makefile
sed -i -r -e 's/css//' Makefile
sed -i -e 's/throw (Exception)//' html/CharsetConverter.h
sed -i -e 's/throw (Exception)//' html/CharsetConverter.cc
echo 'int main() {}' > htmlcxx.cc
make -j4
make install-strip
cd ..
rm -rf htmlcxx-0.87

wget -nc http://silvercoders.com/download/3rdparty/libcharsetdetect-master.tar.bz2
echo "6f9adaf7b6130bee6cfac179e3406cdb933bc83f  libcharsetdetect-master.tar.bz2" | shasum -c
tar -xjvf libcharsetdetect-master.tar.bz2
cd libcharsetdetect-master
cmake -DCMAKE_CXX_STANDARD=17 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix" .
cmake --build . --config Release --target install
cd ..
rm -rf libcharsetdetect-master
rm libcharsetdetect-master.tar.bz2


git clone https://github.com/docwire/wv2.git
cd wv2
mkdir build
cd build
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix" ..
make install
cd ..
wget http://silvercoders.com/download/3rdparty/wv2-0.2.3_patched_4-private_headers.tar.bz2
echo "6bb3959d975e483128623ee3bff3fba343f096c7  wv2-0.2.3_patched_4-private_headers.tar.bz2" | shasum -c
tar -xjvf wv2-0.2.3_patched_4-private_headers.tar.bz2
mv wv2-0.2.3_patched_4-private_headers/*.h $deps_prefix/include/wv2/
cd ..
rm -rf wv2

wget -nc http://www.codesink.org/download/mimetic-0.9.7.tar.gz
echo "568557bbf040be2b17595431c9b0992c32fae6ed  mimetic-0.9.7.tar.gz" | shasum -c
tar -zxvf mimetic-0.9.7.tar.gz
wget -nc http://silvercoders.com/download/3rdparty/mimetic-0.9.7-patches.tar.gz
echo "7e203fb2b2d404ae356674e7dfc293e78fe3362b  mimetic-0.9.7-patches.tar.gz" | shasum -c
tar -xvzf mimetic-0.9.7-patches.tar.gz
cd mimetic-0.9.7
patch -p1 -i ../mimetic-0.9.7-patches/register_keyword.patch
patch -p1 -i ../mimetic-0.9.7-patches/ContTokenizer.patch
patch -p1 -i ../mimetic-0.9.7-patches/macro_string.patch
patch -p1 -i ../mimetic-0.9.7-patches/mimetic_pointer_comparison.patch
./configure CXXFLAGS=-std=c++17 --prefix="$deps_prefix"
make -j4
make install-strip
cd ..
rm -rf mimetic-0.9.7
rm -rf mimetic-0.9.7-patches

git clone https://github.com/libyal/libbfio.git
cd libbfio
git checkout 3bb082c
./synclibs.sh
autoreconf -i
./configure --prefix="$deps_prefix"
make -j4
make install
cd ..
rm -rf libbfio

git clone https://github.com/libyal/libpff.git
cd libpff
git checkout 99a86ef
./synclibs.sh
sed -i.bak "s/2.71/2.69/" configure.ac
autoreconf -i
./configure --prefix="$deps_prefix"
make -j4
make install
cd ..
rm -rf libpff

wget -nc http://www.winimage.com/zLibDll/unzip101e.zip && \
unzip -d unzip101e unzip101e.zip && \
cd unzip101e && \
printf 'cmake_minimum_required(VERSION 3.7)\n' >> CMakeLists.txt
printf 'project(Unzip)\n' >> CMakeLists.txt
printf 'set(UNZIP_SRC ioapi.c unzip.c)\n' >> CMakeLists.txt
printf 'add_library(unzip STATIC ${UNZIP_SRC})\n' >> CMakeLists.txt
printf 'install(FILES unzip.h ioapi.h DESTINATION include)\n' >> CMakeLists.txt
printf 'install(TARGETS unzip DESTINATION lib)\n' >> CMakeLists.txt
printf 'target_compile_options(unzip PRIVATE -fPIC)\n' >> CMakeLists.txt
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_C_FLAGS="-I$vcpkg_prefix/include" -DCMAKE_INSTALL_PREFIX:PATH="$deps_prefix" .
cmake --build .
cmake --install .
cd ..
rm -rf unzip101e

wget http://silvercoders.com/download/3rdparty/cmapresources_korean1-2.tar.z
echo "e4e36995cff0331d8bd5ad00c1c1453c24ab4c07  cmapresources_korean1-2.tar.z" | sha1sum -c -
tar -xvf cmapresources_korean1-2.tar.z
mv ak12 $deps_prefix/share/

wget http://silvercoders.com/download/3rdparty/cmapresources_japan1-6.tar.z
echo "9467d7ed73c16856d2a49b5897fc5ea477f3a111  cmapresources_japan1-6.tar.z" | sha1sum -c -
tar -xvf cmapresources_japan1-6.tar.z
mv aj16 $deps_prefix/share/

wget http://silvercoders.com/download/3rdparty/cmapresources_gb1-5.tar.z
echo "56e6cbd9e053185f9e00118e54fd5159ca118b39  cmapresources_gb1-5.tar.z" | sha1sum -c -
tar -xvf cmapresources_gb1-5.tar.z
mv ag15 $deps_prefix/share/

wget http://silvercoders.com/download/3rdparty/cmapresources_cns1-6.tar.z
echo "80c92cc904c9189cb9611741b913ffd22bcd4036  cmapresources_cns1-6.tar.z" | sha1sum -c -
tar -xvf cmapresources_cns1-6.tar.z
mv ac16 $deps_prefix/share/

wget http://silvercoders.com/download/3rdparty/mappingresources4pdf_2unicode_20091116.tar.Z
echo "aaf44cb1e5dd2043c932e641b0e41432aee2ca0d  mappingresources4pdf_2unicode_20091116.tar.Z" | sha1sum -c -
tar -xvf mappingresources4pdf_2unicode_20091116.tar.Z
mv ToUnicode $deps_prefix/share/

mkdir -p build
cd build
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_TOOLCHAIN_FILE="$vcpkg_toolchain" -DCMAKE_PREFIX_PATH="$deps_prefix" ..
cmake --build .
cmake --build . --target doxygen install
cd ..

cd build
mkdir -p tessdata
cd tessdata
wget -nc https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/eng.traineddata
wget -nc https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/osd.traineddata
wget -nc https://github.com/tesseract-ocr/tessdata_fast/raw/4.1.0/pol.traineddata
cd ..
if [[ "$OSTYPE" == "darwin"* ]]; then
	cp $deps_prefix/lib/libwv2.4.dylib .
	cp $vcpkg_prefix/lib/libpodofo.0.9.8.dylib .
	cp $deps_prefix/lib/libhtmlcxx.3.dylib .
	cp $deps_prefix/lib/libcharsetdetect.dylib .
	cp $deps_prefix/lib/libmimetic.0.dylib .
	cp $deps_prefix/lib/libbfio.1.dylib .
	cp $deps_prefix/lib/libpff.1.dylib .
else
	cp $deps_prefix/lib/libwv2.so.4 .
	cp $vcpkg_prefix/lib/libpodofo.so.0.9.8 .
	cp $deps_prefix/lib/libhtmlcxx.so.3 .
	cp $deps_prefix/lib/libcharsetdetect.so .
	cp $deps_prefix/lib/libmimetic.so.0 .
	cp $deps_prefix/lib/libbfio.so.1 .
	cp $deps_prefix/lib/libpff.so.1 .
fi
mkdir -p resources
cd resources
cp $deps_prefix/share/ac16/CMap/* .
cp $deps_prefix/share/ag15/CMap/* .
cp $deps_prefix/share/aj16/CMap/* .
cp $deps_prefix/share/ak12/CMap/* .
cp $deps_prefix/share/ToUnicode/* .
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
elif [[ "$OSTYPE" != "msys"* ]]; then
	arch=x64_mingw
else
	arch=x86_64_linux
fi
tar -cjvf doctotext-$version-$arch.tar.bz2 build
sha1sum doctotext-$version-$arch.tar.bz2 > doctotext-$version-$arch.tar.bz2.sha1
