vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/libyal/libbfio/releases/download/20240414/libbfio-alpha-20240414.tar.gz"
	FILENAME "libbfio-alpha-20240414.tar.gz"
	SHA512 56c7caa77f0f8b81ea54bdc5579c2fe4fe1cb1660701f0c5bf6d95817cd7dd3992f3451fe2fbd49b7aa29a59f6023af053f612c749b0519dd4b7e61d2d7059bc
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
