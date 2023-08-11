vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/libyal/libbfio/releases/download/20221025/libbfio-alpha-20221025.tar.gz"
	FILENAME "libbfio-alpha-20221025.tar.gz"
	SHA512 ab4be7faae7172b74f117e422f3b72a0705fbfe91f3bf7fb8b5796fae8428ca7d242487eb61b3cf08867209c64949c544a4ed21bf7feb6908fd187e3e4fadd50
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

if (VCPKG_TARGET_IS_WINDOWS)
	vcpkg_execute_required_process(
                COMMAND "devenv.exe"
                "libbfio.sln"
                /Upgrade
                WORKING_DIRECTORY ${SOURCE_PATH}/msvscpp
                LOGNAME upgrade-libbfio-${TARGET_TRIPLET}
        )
	vcpkg_install_msbuild(
		SOURCE_PATH "${SOURCE_PATH}"
		PROJECT_SUBPATH "msvscpp/libbfio.sln"
	)
else()
	vcpkg_configure_make(
		SOURCE_PATH "${SOURCE_PATH}"
		COPY_SOURCE
	)
	vcpkg_install_make()
endif()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(INSTALL "${SOURCE_PATH}/COPYING.LESSER" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
