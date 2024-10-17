if(DEFINED ENV{SOURCE_PATH})
	set(SOURCE_PATH $ENV{SOURCE_PATH})
else()
	vcpkg_from_github(
		OUT_SOURCE_PATH SOURCE_PATH
		REPO docwire/docwire
		HEAD_REF master
	)
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
	FEATURES
		asan ADDRESS_SANITIZER
		tsan THREAD_SANITIZER
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS ${FEATURE_OPTIONS}
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP FALSE)

vcpkg_cmake_install()

if(VCPKG_TARGET_IS_WINDOWS)
	set(script_suffix .bat)
else()
	set(script_suffix .sh)
endif()

vcpkg_copy_tools(
	TOOL_NAMES docwire
	SEARCH_DIR ${CURRENT_PACKAGES_DIR}/bin
	DESTINATION ${CURRENT_PACKAGES_DIR}/tools
)
vcpkg_copy_tools(
	TOOL_NAMES docwire
	SEARCH_DIR ${CURRENT_PACKAGES_DIR}/debug/bin
	DESTINATION ${CURRENT_PACKAGES_DIR}/debug/tools
)
vcpkg_clean_executables_in_bin(FILE_NAMES docwire)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/LICENSE ${SOURCE_PATH}/doc/COPYING.GPLv2)

function(run_tests build_type)
	set(triplet_build_type ${TARGET_TRIPLET}-${build_type})
	message(STATUS "Testing ${triplet_build_type}")

	file(WRITE "${CURRENT_BUILDTREES_DIR}/${triplet_build_type}/share/flan-t5-large-ct2-int8.path" "${CURRENT_INSTALLED_DIR}/share/flan-t5-large-ct2-int8")
	file(WRITE "${CURRENT_BUILDTREES_DIR}/${triplet_build_type}/share/tessdata-fast.path" "${CURRENT_INSTALLED_DIR}/share/tessdata-fast")
	file(WRITE "${CURRENT_BUILDTREES_DIR}/${triplet_build_type}/share/libmagic/misc/magic.mgc.path" "${CURRENT_INSTALLED_DIR}/share/libmagic/misc/magic.mgc")

	set(valgrind_command "")
	if(MEMCHECK_ENABLED)
		set(valgrind_command valgrind --leak-check=full --gen-suppressions=all --suppressions=${SOURCE_PATH}/tools/valgrind_suppressions.txt)
	elseif(CALLGRIND_ENABLED)
		set(valgrind_command valgrind --tool=callgrind)
	elseif(HELGRIND_ENABLED)
		set(valgrind_command valgrind --tool=helgrind --gen-suppressions=all --suppressions=${SOURCE_PATH}/tools/valgrind_suppressions.txt)
	endif()
	if (valgrind_command)
		set(valgrind_command ${valgrind_command} --trace-children=yes --error-exitcode=1)
	endif()
	if (valgrind_command)
		message(STATUS "Using valgrind: ${valgrind_command}")
	endif()

	set(additional_ctest_args "")
	if (VCPKG_TARGET_IS_LINUX AND (THREAD_SANITIZER OR MEMCHECK_ENABLED OR HELGRIND_ENABLED))
		message(STATUS "Skipping tests that use model runner (Thread Sanitizer or Helgrind) on Linux")
		set(additional_ctest_args --label-exclude uses_model_runner)
	endif()

	vcpkg_execute_required_process(
		COMMAND ${valgrind_command} "ctest"
			-V
			--no-tests=error
			--label-regex is_api_test|is_example
			${additional_ctest_args}
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-${PORT}-${triplet_build_type}
	)
endfunction()

function(run_all_tests)
	file(WRITE "${CURRENT_PACKAGES_DIR}/share/flan-t5-large-ct2-int8.path" "${CURRENT_INSTALLED_DIR}/share/flan-t5-large-ct2-int8")
	file(WRITE "${CURRENT_PACKAGES_DIR}/share/tessdata-fast.path" "${CURRENT_INSTALLED_DIR}/share/tessdata-fast")
	file(WRITE "${CURRENT_PACKAGES_DIR}/share/libmagic/misc/magic.mgc.path" "${CURRENT_INSTALLED_DIR}/share/libmagic/misc/magic.mgc")
	if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL debug)
		if(VCPKG_TARGET_IS_LINUX)
			set(BACKUP_LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
			set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/debug/lib:${CURRENT_INSTALLED_DIR}/debug/lib")
		endif()
		if(VCPKG_TARGET_IS_WINDOWS)
			set(BACKUP_PATH $ENV{PATH})
			file(TO_NATIVE_PATH "${CURRENT_PACKAGES_DIR}/debug/bin;${CURRENT_INSTALLED_DIR}/debug/bin" path_addon_native)
			set(ENV{PATH} "${BACKUP_PATH};${path_addon_native}")
			message(status "PATH=$ENV{PATH}")
		endif()
		if(VCPKG_TARGET_IS_OSX)
			set(BACKUP_DYLD_FALLBACK_LIBRARY_PATH $ENV{DYLD_FALLBACK_LIBRARY_PATH})
			set(ENV{DYLD_FALLBACK_LIBRARY_PATH} "${BACKUP_DYLD_FALLBACK_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/debug/lib:${CURRENT_INSTALLED_DIR}/debug/lib")
		endif()
		run_tests(dbg)
		if(VCPKG_TARGET_IS_LINUX)
			set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}")
		endif()
		if(VCPKG_TARGET_IS_WINDOWS)
			set(ENV{PATH} "${BACKUP_PATH}")
		endif()
		if(VCPKG_TARGET_IS_OSX)
			set(ENV{DYLD_FALLBACK_LIBRARY_PATH} "${BACKUP_DYLD_FALLBACK_LIBRARY_PATH}")
		endif()
	endif()
	if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL release)
		if(VCPKG_TARGET_IS_LINUX)
			set(BACKUP_LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
			set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/lib:${CURRENT_INSTALLED_DIR}/lib")
		endif()
		if(VCPKG_TARGET_IS_WINDOWS)
			set(BACKUP_PATH $ENV{PATH})
			set(ENV{PATH} "${BACKUP_PATH};${CURRENT_PACKAGES_DIR}/bin;${CURRENT_INSTALLED_DIR}/bin")
		endif()
		if(VCPKG_TARGET_IS_OSX)
			set(BACKUP_DYLD_FALLBACK_LIBRARY_PATH $ENV{DYLD_FALLBACK_LIBRARY_PATH})
			set(ENV{DYLD_FALLBACK_LIBRARY_PATH} "${BACKUP_DYLD_FALLBACK_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/lib:${CURRENT_INSTALLED_DIR}/lib")
		endif()
		run_tests(rel)
		if(VCPKG_TARGET_IS_LINUX)
			set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}")
		endif()
		if(VCPKG_TARGET_IS_WINDOWS)
			set(ENV{PATH} "${BACKUP_PATH}")
		endif()
		if(VCPKG_TARGET_IS_OSX)
			set(ENV{DYLD_FALLBACK_LIBRARY_PATH} "${BACKUP_DYLD_FALLBACK_LIBRARY_PATH}")
		endif()
	endif()
	file(REMOVE "${CURRENT_PACKAGES_DIR}/share/flan-t5-large-ct2-int8.path")
	file(REMOVE "${CURRENT_PACKAGES_DIR}/share/tessdata-fast.path")
	file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/libmagic")
endfunction()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS_NO_CMAKE
	FEATURES
		tests TESTS_ENABLED
		memcheck MEMCHECK_ENABLED
		helgrind HELGRIND_ENABLED
		callgrind CALLGRIND_ENABLED
)

if (TESTS_ENABLED)
	run_all_tests()
endif()
