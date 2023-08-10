vcpkg_download_distfile(ARCHIVE
	URLS "http://www.winimage.com/zLibDll/unzip101e.zip"
	FILENAME "unzip101e.zip"
	SHA512 f09ca22be674ef3813428120f756a74f2b46f83035a4bb1ad7f39b41f1ff1f6d83d5c7d07e9d2b8eb6de543209ea7eb4b2e36ecb2a2758a40477189ee5658339
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
	NO_REMOVE_ONE_LEVEL
)

file(COPY
	${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
	DESTINATION ${SOURCE_PATH}
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/copyright" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
