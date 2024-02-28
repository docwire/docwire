set -e

if [[ "$OSTYPE" != "darwin"* && "$OSTYPE" != "linux"* ]]; then
    echo "Error: Not supported OS type." >&2
    exit 1
fi

if [[ "$DOWNLOAD_VCPKG" != "0" ]]; then
	if [[ -d vcpkg ]]; then
		echo "Error: vcpkg directory already exists. Remove it or set DOWNLOAD_VCPKG=0 if you are sure directory content is correct." >&2
		exit 1
	fi
	git clone https://github.com/microsoft/vcpkg.git
	cd vcpkg
	git checkout tags/2024.01.12
	if [[ "$OSTYPE" == "darwin"* ]]; then
		# Temporary workaround to upgrade Meson to version 1.3.0 due to compatibility issues with certain Python versions.
		# This upgrade is necessary because Meson version 0.63, which is the default in the current vcpkg version, does not work properly with these Python versions.
		# These sed commands manually update the Meson version in the vcpkg-tool-meson port.
		# Note: This is a temporary measure. The upgrade to Meson version 1.3.0 will be officially included in future versions of vcpkg.
		sed -i -e 's/0.63/1.3.0/' ports/vcpkg-tool-meson/vcpkg.json
		sed -i -e 's/0.63.0/1.3.0/' ports/vcpkg-tool-meson/portfile.cmake
		sed -i -e 's/bb91cea0d66d8d036063dedec1f194d663399cdf/7368795d13081d4928a9ba04d48498ca2442624b/' ports/vcpkg-tool-meson/portfile.cmake
		sed -i -e 's/e5888eb35dd4ab5fc0a16143cfbb5a7849f6d705e211a80baf0a8b753e2cf877a4587860a79cad129ec5f3474c12a73558ffe66439b1633d80b8044eceaff2da/b2dc940a8859d6b0af8cb762c896d3188cadc1e65e3c7d922d6cb9a4ed7a1be88cd4d51a8aa140319a75e467816e714c409cf74c71c830bbc5f96bb81c1845ce/' ports/vcpkg-tool-meson/portfile.cmake
		sed -i -e 's/remove-freebsd-pcfile-specialization.patch/#/' ports/vcpkg-tool-meson/portfile.cmake
		sed -i -e "s/intl_factory/packages['intl'] = intl_factory/" ports/vcpkg-tool-meson/meson-intl.patch
	fi
	./bootstrap-vcpkg.sh
	cd ..
fi

if [[ -n "$BINARY_CACHE_GITHUB_TOKEN" ]]; then
	echo "Configuring GitHub packages binary cache."
	NUGET=`./vcpkg/vcpkg fetch nuget | tail -n1`
	echo "Using NuGet: $NUGET"
	OWNER="${GITHUB_REPOSITORY_OWNER:-docwire}"
	echo "Using GitHub owner: $OWNER"
	SOURCE_URL="https://nuget.pkg.github.com/$OWNER/index.json"
	echo "Using cache source: $SOURCE_URL"
	SOURCE_NAME="docwire_github"
	mono "$NUGET" sources add -source "$SOURCE_URL" -storepasswordincleartext -name "$SOURCE_NAME" -username "$BINARY_CACHE_GITHUB_USER" -password "$BINARY_CACHE_GITHUB_TOKEN"
	mono "$NUGET" setapikey "$BINARY_CACHE_GITHUB_TOKEN" -source "$SOURCE_URL"
	export VCPKG_BINARY_SOURCES="clear;nuget,$SOURCE_NAME,readwrite"
	echo "GitHub packages binary cache enabled."
else
	echo "GitHub packages binary cache disabled."
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
	if [[ $(arch) == 'arm64' ]]; then
		VCPKG_TRIPLET=arm64-osx-dynamic
	else
		VCPKG_TRIPLET=x64-osx-dynamic
	fi
else
	VCPKG_TRIPLET=x64-linux-dynamic
fi

if [[ "$SANITIZER" == "address" ]]; then
	FEATURES="[tests,address-sanitizer]"
elif [[ "$SANITIZER" == "thread" ]]; then
	FEATURES="[tests,thread-sanitizer]"
else
	FEATURES="[tests]"
fi

date > ./ports/docwire/.disable_binary_cache
SOURCE_PATH="$PWD" VCPKG_KEEP_ENV_VARS=SOURCE_PATH ./vcpkg/vcpkg --overlay-ports=./ports remove docwire:$VCPKG_TRIPLET
SOURCE_PATH="$PWD" VCPKG_KEEP_ENV_VARS="SOURCE_PATH;DOCWIRE_LOG_VERBOSITY;OPENAI_API_KEY;ASAN_OPTIONS;TSAN_OPTIONS" ./vcpkg/vcpkg --overlay-ports=./ports install docwire$FEATURES:$VCPKG_TRIPLET

if [[ "$EXPORT_VCPKG" != "0" ]]; then
	version=`cat ./vcpkg/installed/$VCPKG_TRIPLET/share/docwire/VERSION`
	./vcpkg/vcpkg --overlay-ports=./ports export docwire:$VCPKG_TRIPLET --raw --output=docwire-$version --output-dir=.

	cat tools/setup_env.sh | sed "s/vcpkg_triplet=.*/vcpkg_triplet=\"$VCPKG_TRIPLET\"/" > docwire-$version/setup_env.sh
	chmod u+x docwire-$version/setup_env.sh

	# test run - relative path
	(
		. docwire-$version/setup_env.sh
		docwire tests/1.pdf
	)
	# test run - absolute path
	(
		. $PWD/docwire-$version/setup_env.sh
		docwire tests/1.doc
	)

	if [[ "$CREATE_ARCHIVE" == "1" ]]; then
		abi_suffix=`cat ./vcpkg/installed/$VCPKG_TRIPLET/share/docwire/abi-id.txt`
		full_suffix="$version-$VCPKG_TRIPLET-$abi_suffix"
		tar -cjvf docwire-$full_suffix.tar.bz2 docwire-$version
		sha1sum docwire-$full_suffix.tar.bz2 > docwire-$full_suffix.tar.bz2.sha1
	fi
fi
