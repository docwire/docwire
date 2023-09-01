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
git checkout tags/2023.01.09
./bootstrap-vcpkg.sh
cd ..

if [[ "$OSTYPE" == "darwin"* ]]; then
	VCPKG_TRIPLET=x64-osx-dynamic
else
	VCPKG_TRIPLET=x64-linux-dynamic
fi

date > ./ports/doctotext/.disable_binary_cache
tar cvf sources-temp.tar --exclude="vcpkg" --exclude='doctotext*.bz2*' --exclude=".git" --exclude="sources-temp.tar" .
SOURCES_ARCHIVE=$PWD/sources-temp.tar VCPKG_KEEP_ENV_VARS=SOURCES_ARCHIVE ./vcpkg/vcpkg --overlay-ports=./ports install doctotext:$VCPKG_TRIPLET

version=`cat ./vcpkg/installed/$VCPKG_TRIPLET/share/doctotext/VERSION`
./vcpkg/vcpkg --overlay-ports=./ports export doctotext:$VCPKG_TRIPLET --raw --output=doctotext-$version --output-dir=.

tar -cjvf doctotext-$version-$VCPKG_TRIPLET.tar.bz2 doctotext-$version
sha1sum doctotext-$version-$VCPKG_TRIPLET.tar.bz2 > doctotext-$version-$VCPKG_TRIPLET.tar.bz2.sha1
