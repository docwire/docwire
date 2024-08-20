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
	./bootstrap-vcpkg.sh
	cd ..
fi

if [[ -n "$BINARY_CACHE_GITHUB_TOKEN" ]]; then
	echo "Configuring GitHub packages binary cache."
	NUGET=`./vcpkg/vcpkg fetch nuget | tail -n1`
	echo "Using NuGet: $NUGET"
	BINARY_CACHE_GITHUB_OWNERS="${BINARY_CACHE_GITHUB_OWNERS:-docwire}"
	for OWNER in $BINARY_CACHE_GITHUB_OWNERS; do
		SOURCE_URL="https://nuget.pkg.github.com/$OWNER/index.json"
		echo "Using cache source: $SOURCE_URL"
		SOURCE_NAME="${OWNER}_github"
		mono "$NUGET" sources add -source "$SOURCE_URL" -storepasswordincleartext -name "$SOURCE_NAME" -username "$BINARY_CACHE_GITHUB_USER" -password "$BINARY_CACHE_GITHUB_TOKEN"
		mono "$NUGET" setapikey "$BINARY_CACHE_GITHUB_TOKEN" -source "$SOURCE_URL"
		VCPKG_BINARY_SOURCES+=";nuget,$SOURCE_NAME,readwrite"
	done
	export VCPKG_BINARY_SOURCES="clear$VCPKG_BINARY_SOURCES"
	echo "Using binary sources: $VCPKG_BINARY_SOURCES"
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

if [[ "$DEBUG" == "1" ]]; then
	export DOCWIRE_LOG_VERBOSITY="debug"
	VCPKG_DEBUG_OPTION="--debug"
fi

date > ./ports/docwire/.disable_binary_cache
SOURCE_PATH="$PWD" VCPKG_KEEP_ENV_VARS=SOURCE_PATH ./vcpkg/vcpkg --overlay-ports=./ports remove docwire:$VCPKG_TRIPLET
SOURCE_PATH="$PWD" VCPKG_KEEP_ENV_VARS="SOURCE_PATH;DOCWIRE_LOG_VERBOSITY;OPENAI_API_KEY;ASAN_OPTIONS;TSAN_OPTIONS" ./vcpkg/vcpkg --overlay-ports=./ports install $VCPKG_DEBUG_OPTION docwire$FEATURES:$VCPKG_TRIPLET

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
