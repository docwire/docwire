#!/bin/bash
set -e

if [[ -z "$1" ]]; then
    vcpkg_triplet="" # this line can be replaced during build stage
    if [[ -z "$vcpkg_triplet" ]]; then
        echo "Usage: . ./setup_env.sh [<directory>]" >&2
        return 1 || exit 1
    fi
    installed_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd "installed/$vcpkg_triplet" && pwd )" # bash version
else
    installed_dir="$( cd "$1" && pwd )"
fi

if [[ ! -f "$installed_dir/tools/docwire" ]]; then
    echo "Error: directory \"$installed_dir\" does not look correctly." >&2
    return 1 || exit 1
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    export PATH="$installed_dir/tools:$PATH"
    export DYLD_FALLBACK_LIBRARY_PATH="$installed_dir/lib:$installed_dir/lib/docwire_system_libraries:$DYLD_FALLBACK_LIBRARY_PATH"
    export CPLUS_INCLUDE_PATH="$installed_dir/include:$CPLUS_INCLUDE_PATH"
    export LIBRARY_PATH="$installed_dir/lib:$installed_dir/lib/docwire_system_libraries:$LIBRARY_PATH"
    export OPENSSL_MODULES="$installed_dir/lib/ossl-modules"
else
    export PATH="$installed_dir/tools:$PATH"
    export LD_LIBRARY_PATH="$installed_dir/lib:$installed_dir/lib/docwire_system_libraries:$LD_LIBRARY_PATH"
    export CPLUS_INCLUDE_PATH="$installed_dir/include:$CPLUS_INCLUDE_PATH"
    export LIBRARY_PATH="$installed_dir/lib:$LIBRARY_PATH"
    export OPENSSL_MODULES="$installed_dir/lib/ossl-modules"
fi
echo "The environment has been configured for DocWire SDK installed in $installed_dir." 2>&1
