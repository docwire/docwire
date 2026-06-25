vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/libyal/libbfio/releases/download/20260623/libbfio-alpha-20260623.tar.gz"
	FILENAME "libbfio-alpha-20260623.tar.gz"
	SHA512 6130f8687fe258a5bed366dd48b5df346dfb815261907f4ad7e29d8f1c1c27fd0e2f10204e928ef9cc723b6c3033a4148e0c20ec048e347060d5d4de62c638f4
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
	file(GLOB_RECURSE project_files ${SOURCE_PATH}/*.sln ${SOURCE_PATH}/*.vcxproj)
	foreach(file ${project_files})
		vcpkg_replace_string(${file} Release|Win32 Release|x64)
		vcpkg_replace_string(${file} VSDebug|Win32 Debug|x64)
		vcpkg_replace_string(${file} MachineX86 MachineX64)
	endforeach()
	vcpkg_install_msbuild(
		SOURCE_PATH "${SOURCE_PATH}"
		PROJECT_SUBPATH "msvscpp/libbfio.sln"
	)
	file(INSTALL ${SOURCE_PATH}/include/ DESTINATION ${CURRENT_PACKAGES_DIR}/include FILES_MATCHING PATTERN "*.h")
else()
	vcpkg_configure_make(
		SOURCE_PATH "${SOURCE_PATH}"
		COPY_SOURCE
	)
	vcpkg_install_make()
	vcpkg_fixup_pkgconfig()
endif()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(INSTALL "${SOURCE_PATH}/COPYING.LESSER" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
