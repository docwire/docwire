vcpkg_download_distfile(ARCHIVE
	URLS "https://github.com/libyal/libpff/releases/download/20260926/libpff-alpha-20260926.tar.gz"
	FILENAME "libpff-alpha-20260926.tar.gz"
	SHA512 5a6277789371be256aeed8f568b0364e3da54c1f3c4d04b3fb74c39ab5e9de129fb5c65d1a0e4ec2c68f096a6f65a372059a7e5299ccfbb520ee96a4a4c5e8ec
)

vcpkg_extract_source_archive_ex(
	OUT_SOURCE_PATH SOURCE_PATH
	ARCHIVE ${ARCHIVE}
)

if (VCPKG_TARGET_IS_WINDOWS)
	# The pypff project requires Python.h, which is not a dependency of this port.
	# Remove it from the solution before devenv upgrades the old solution.
	file(STRINGS "${SOURCE_PATH}/msvscpp/libpff.sln" _libpff_sln_lines)
	set(_libpff_sln_lines_filtered)
	set(_skip_pypff FALSE)
	foreach(_line IN LISTS _libpff_sln_lines)
		if(_skip_pypff)
			if(_line STREQUAL "EndProject")
				set(_skip_pypff FALSE)
			endif()
			continue()
		endif()

		if(_line MATCHES "Project\\(.*\\) = \"pypff\",")
			set(_skip_pypff TRUE)
			continue()
		endif()

		if(_line MATCHES "\\{E221DB4C-B254-47CB-993D-DC7FED580DA1\\}")
			continue()
		endif()

		list(APPEND _libpff_sln_lines_filtered "${_line}")
	endforeach()
	list(JOIN _libpff_sln_lines_filtered "\r\n" _libpff_sln_content)
	file(WRITE "${SOURCE_PATH}/msvscpp/libpff.sln" "${_libpff_sln_content}")

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
		vcpkg_replace_string(${file} [[..\..\..\zlib]] [[..\..\zlib-1.3.2]])
	endforeach()
	vcpkg_download_distfile(ZLIB_ARCHIVE
		URLS "https://zlib.net/zlib132.zip"
		FILENAME "zlib132.zip"
		SHA512 3d673df9aa2085d0349b673f25bacfa79807232f6059970a8b7f81110233bc33076f224237e208a8f176bdebd977f5361de03edf35125e01bbb55552e92d53aa
	)
	file(ARCHIVE_EXTRACT INPUT ${ZLIB_ARCHIVE} DESTINATION ${SOURCE_PATH})
	vcpkg_install_msbuild(
		SOURCE_PATH "${SOURCE_PATH}"
		PROJECT_SUBPATH "msvscpp/libpff.sln"
	)
	# The libpff solution transitively builds the libyal helper libraries
	# (libbfio, libcdata, libcerror, ...) and copies their import/static
	# libraries into this package. Those libraries are already provided by
	# the dedicated libbfio overlay port, so remove the duplicate artifacts
	# installed here to avoid conflicting files with libbfio.
	set(libyal_helper_libraries
		libbfio
		libcdata
		libcerror
		libclocale
		libcnotify
		libcsplit
		libcthreads
		libfdata
		libfcache
		libfdatetime
		libfguid
		libfmapi
		libfole
		libfsntfs
		libfwnt
		libregf
		libuna
	)
	foreach(libyal_helper_library ${libyal_helper_libraries})
		file(REMOVE
			"${CURRENT_PACKAGES_DIR}/lib/${libyal_helper_library}.lib"
			"${CURRENT_PACKAGES_DIR}/debug/lib/${libyal_helper_library}.lib"
		)
	endforeach()
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
