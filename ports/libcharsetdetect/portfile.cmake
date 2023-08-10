vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/batterseapower/libcharsetdetect/archive/7b77a7a9d6359480c2a4c0b6f67610bc79398fea.zip"
	FILENAME "libcharsetdetect-7b77a7a9d6359480c2a4c0b6f67610bc79398fea.zip"
	SHA512 cc5d5ec413a4dd9543fabd832f18576e89687e4b494d2ec7d41f6f6d307c723033a20a17fcdad9ada9916b0f74beb1b59eb42e32b0fe2993fd85a1faaef5842b
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

#file(COPY
#	${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
#	DESTINATION ${SOURCE_PATH}
#)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/copyright" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
