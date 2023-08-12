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
	vcpkg_replace_string(${SOURCE_PATH}/msvscpp/zlib.vcproj [[..\..\..\zlib]] [[..\..\zlib-1.2.13]])
	vcpkg_download_distfile(ZLIB_ARCHIVE
		URLS "http://zlib.net/zlib1213.zip"
		FILENAME "http://zlib.net/zlib1213.zip"
		SHA512 4a1c487db9fd442a4c655dd91b9fbea89af08da189154cae575a095d1e4c10e98283a8e11bb511c164e5e9dcf2f38e5996bda3e17d3f09c02ab5ce4e533505fb
	)
	file(ARCHIVE_EXTRACT INPUT ${ZLIB_ARCHIVE} DESTINATION ${SOURCE_PATH})
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
