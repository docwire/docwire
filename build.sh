set -e

if [[ "$OSTYPE" == "darwin"* ]]; then
	brew install md5sha1sum automake autogen doxygen
elif [[ "$OSTYPE" == "linux"* ]]; then
	if [[ "$GITHUB_ACTIONS" == "true" ]]; then
		sudo apt-get install -y autopoint
		sudo apt-get install -y doxygen
	fi
else
	echo "Unknown OS type." >&2
	exit 1
fi

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout tags/2023.11.20
./bootstrap-vcpkg.sh
cd ..

if [[ "$OSTYPE" == "darwin"* ]]; then
	VCPKG_TRIPLET=x64-osx-dynamic
else
	VCPKG_TRIPLET=x64-linux-dynamic
fi

date > ./ports/docwire/.disable_binary_cache
SOURCE_PATH="$PWD" VCPKG_KEEP_ENV_VARS=SOURCE_PATH ./vcpkg/vcpkg --overlay-ports=./ports install docwire:$VCPKG_TRIPLET

version=`cat ./vcpkg/installed/$VCPKG_TRIPLET/share/docwire/VERSION`
./vcpkg/vcpkg --overlay-ports=./ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

echo "\$(dirname \"\$0\")/installed/$VCPKG_TRIPLET/tools/docwire.sh \"\$@\"" > docwire-$version/docwire.sh
chmod u+x docwire-$version/docwire.sh

tar -cjvf docwire-$version-$VCPKG_TRIPLET.tar.bz2 docwire-$version
sha1sum docwire-$version-$VCPKG_TRIPLET.tar.bz2 > docwire-$version-$VCPKG_TRIPLET.tar.bz2.sha1
