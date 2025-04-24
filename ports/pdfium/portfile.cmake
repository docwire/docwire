vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL https://pdfium.googlesource.com/pdfium.git
    REF c1efd963d2f714b1b63ba30ab4d01150fbe1d5d8
)

vcpkg_from_git(
	OUT_SOURCE_PATH BUILD_DIR_SOURCE_PATH
	URL https://chromium.googlesource.com/chromium/src/build.git
	REF 49491ecc7ae52b980a0bd3ad06cbde9c93af09ab
)
file(RENAME ${BUILD_DIR_SOURCE_PATH} ${SOURCE_PATH}/build)

vcpkg_from_git(
	OUT_SOURCE_PATH FAST_FLOAT_SOURCE_PATH
	URL https://chromium.googlesource.com/external/github.com/fastfloat/fast_float.git
	REF cb1d42aaa1e14b09e1452cfdef373d051b8c02a4
)
file(RENAME ${FAST_FLOAT_SOURCE_PATH} ${SOURCE_PATH}/third_party/fast_float/src)

vcpkg_from_git(
	OUT_SOURCE_PATH ABSEIL_CPP_SOURCE_PATH
	URL https://chromium.googlesource.com/chromium/src/third_party/abseil-cpp.git
	REF 1e8b41f1ecf3191724e85acfa03abf4175038860
)
file(RENAME ${ABSEIL_CPP_SOURCE_PATH} ${SOURCE_PATH}/third_party/abseil-cpp)

file(COPY
	${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
	DESTINATION ${SOURCE_PATH}
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
