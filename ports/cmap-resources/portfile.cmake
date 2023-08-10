vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/adobe-type-tools/cmap-resources/archive/refs/tags/20230622.tar.gz"
	FILENAME "cmap_resources-20230622.tar.gz"
	SHA512 1434b5481e91828268feddab8bb83857412c460361c4a8dad6cbb368512e11c3684b914f79d602957aae1266889636d913bb4798b01051c0fa6d5e203bae8c05
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

file(COPY
        ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
        DESTINATION ${SOURCE_PATH}
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/include/${PORT})
file(TOUCH ${CURRENT_PACKAGES_DIR}/include/${PORT}/placeholder)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
file(INSTALL "${SOURCE_PATH}/LICENSE.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
