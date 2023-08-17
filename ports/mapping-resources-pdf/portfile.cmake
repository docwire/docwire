vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/adobe-type-tools/mapping-resources-pdf/archive/refs/tags/20230118.tar.gz"
	FILENAME "mapping_resources_pdf-20230118.tar.gz"
	SHA512 b0196ad9dbf38c4d80db0dcffc28556692569c6f5e1e8bdce00067e1ecc8ed02312e27c2697f2f6d5eb9fb87c61975289641ba100fdb4b77a80cba75146ae665
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
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
