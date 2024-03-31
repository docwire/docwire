if(DEBUG)
	message(STATUS "vcpkg was called with --debug argument")
else()
	message(STATUS "vcpkg was not called with --debug argument")
endif()

if(DEFINED ENV{SOURCE_PATH})
	set(SOURCE_PATH $ENV{SOURCE_PATH})
else()
	vcpkg_from_github(
		OUT_SOURCE_PATH SOURCE_PATH
		REPO docwire/docwire
		REF 2024.01.05
		SHA512 8d16d7172897775adbbe8891ddb7326708a0e7d0a23961294ec1251af95b4b0af5689066594495b9327d50183080d98baa1d6f2002cdc1b285cef3683bc7af42
		HEAD_REF master
	)
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
	FEATURES
		address-sanitizer ADDRESS_SANITIZER
		thread-sanitizer THREAD_SANITIZER
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

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/tools)
file(RENAME
	"${CURRENT_PACKAGES_DIR}/bin/docwire${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/tools/docwire${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
if (EXISTS "${CURRENT_PACKAGES_DIR}/bin/docwire${script_suffix}") # Removed in new release
	file(RENAME "${CURRENT_PACKAGES_DIR}/bin/docwire${script_suffix}" "${CURRENT_PACKAGES_DIR}/tools/docwire${script_suffix}")
endif()
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/tools)
file(RENAME
	"${CURRENT_PACKAGES_DIR}/debug/bin/docwire${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/debug/tools/docwire${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
if (EXISTS "${CURRENT_PACKAGES_DIR}/debug/bin/docwire${script_suffix}") # Removed in new release
	file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/docwire${script_suffix}" "${CURRENT_PACKAGES_DIR}/debug/tools/docwire${script_suffix}")
endif()
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static" OR NOT VCPKG_TARGET_IS_WINDOWS)
	file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/LICENSE ${SOURCE_PATH}/doc/COPYING.GPLv2)

function(run_tests build_type)
	set(triplet_build_type ${TARGET_TRIPLET}-${build_type})
	message(STATUS "Testing ${triplet_build_type}")

	set(valgrind_command "")
	if(MEMCHECK_ENABLED)
		set(valgrind_command valgrind --leak-check=full --gen-suppressions=yes --suppressions=${SOURCE_PATH}/tools/valgrind_suppressions.txt)
	elseif(CALLGRIND_ENABLED)
		set(valgrind_command valgrind --tool=callgrind)
	elseif(HELGRIND_ENABLED)
		set(valgrind_command valgrind --tool=helgrind --gen-suppressions=yes --suppressions=${SOURCE_PATH}/tools/valgrind_suppressions.txt)
	endif()
	if (valgrind_command)
		set(valgrind_command ${valgrind_command} --trace-children=yes --error-exitcode=1 --verbose)
	endif()
	if (valgrind_command)
		message(STATUS "Using valgrind: ${valgrind_command}")
	endif()

	vcpkg_execute_required_process(
		COMMAND ${valgrind_command} "ctest"
			-V
			--no-tests=error
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-${PORT}-${triplet_build_type}
	)
endfunction()

function(run_all_tests)
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
