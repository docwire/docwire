vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/libyal/libpff/releases/download/20211114/libpff-alpha-20211114.tar.gz"
	FILENAME "libpff-alpha-20211114.tar.gz"
	SHA512 ad2cf4b0841c448b60738cd2f340868c0f11eb34167bfe5b093645a2a080d694e199afe4fef5eeea1016487820132be33f8e51910d2142ff032320ad2dbeb59d
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

if (VCPKG_TARGET_IS_WINDOWS)
	vcpkg_execute_required_process(
		COMMAND "devenv.exe"
		"libpff.sln"
		/Upgrade
		WORKING_DIRECTORY ${SOURCE_PATH}/msvscpp
		LOGNAME upgrade-libpff-${TARGET_TRIPLET}
	)
	file(GLOB_RECURSE project_files ${SOURCE_PATH}/*.sln ${SOURCE_PATH}/*.vcxproj)
	foreach(file ${project_files})
		vcpkg_replace_string(${file} Release|Win32 Release|x64)
		vcpkg_replace_string(${file} VSDebug|Win32 Debug|x64)
		vcpkg_replace_string(${file} MachineX86 MachineX64)
	endforeach()
	vcpkg_replace_string(${SOURCE_PATH}/msvccpp/zlib.vcproj ..\..\.. ..\..)
	vcpkg_install_msbuild(
		SOURCE_PATH "${SOURCE_PATH}"
		PROJECT_SUBPATH "msvscpp/libpff.sln"
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
