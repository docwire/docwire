#!/usr/bin/env bash
set -e  # stop on first error

# ==========================
# CONFIGURATION — adjust paths
# ==========================
VCPKG_TOOLCHAIN=/home/reeshabh/coderepos/docwire/vcpkg/scripts/buildsystems/vcpkg.cmake
VCPKG_TRIPLET=x64-linux-dynamic
DOCWIRE_DIR=/home/reeshabh/coderepos/docwire/vcpkg/installed/x64-linux-dynamic/share/docwire
BUILD_DIR=./build
DEMO_EXEC=demo


# ==========================
# CLEAN BUILD
# ==========================
echo "Cleaning old build folder..."
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

# ==========================
# CONFIGURE CMAKE
# ==========================
echo "Configuring CMake..."
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_TOOLCHAIN" \
  -DVCPKG_TARGET_TRIPLET="$VCPKG_TRIPLET" \
  -Ddocwire_DIR="$DOCWIRE_DIR" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# ==========================
# BUILD
# ==========================
echo "Building project..."
cmake --build .

# ==========================
# SET LIBRARY PATH
# ==========================
export LD_LIBRARY_PATH=/home/reeshabh/coderepos/docwire/vcpkg/installed/x64-linux-dynamic/lib:$LD_LIBRARY_PATH

# ==========================
# RUN DEMO
# ==========================
# echo "Running demo..."
# ./$DEMO_EXEC
