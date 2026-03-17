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
		helgrind HELGRIND_ENABLED
)

if(DEFINED ENV{CMAKE_MESSAGE_LOG_LEVEL})
	list(APPEND FEATURE_OPTIONS "-DCMAKE_MESSAGE_LOG_LEVEL=$ENV{CMAKE_MESSAGE_LOG_LEVEL}")
endif()

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS ${FEATURE_OPTIONS}
)

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
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/docwire)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/LICENSE ${SOURCE_PATH}/doc/COPYING.GPLv2)

function(run_tests build_type)
	set(triplet_build_type ${TARGET_TRIPLET}-${build_type})
	message(STATUS "Testing ${triplet_build_type}")

	set(valgrind_command "")
	if(MEMCHECK_ENABLED)
		set(valgrind_command valgrind --leak-check=full --gen-suppressions=all --suppressions=${SOURCE_PATH}/tools/memcheck_common.supp)
	elseif(CALLGRIND_ENABLED)
		set(valgrind_command valgrind --tool=callgrind)
	elseif(HELGRIND_ENABLED)
		set(valgrind_command valgrind --tool=helgrind --gen-suppressions=all --suppressions=${SOURCE_PATH}/tools/helgrind_common.supp)
		if(build_type STREQUAL dbg)
			set(valgrind_command ${valgrind_command} --suppressions=${SOURCE_PATH}/tools/helgrind_debug.supp)
		elseif(build_type STREQUAL rel)
			set(valgrind_command ${valgrind_command} --suppressions=${SOURCE_PATH}/tools/helgrind_release.supp)
		endif()
	endif()
	if (valgrind_command)
		set(valgrind_command ${valgrind_command} --trace-children=yes --error-exitcode=1 --num-callers=100)
	endif()
	if (valgrind_command)
		message(STATUS "Using valgrind: ${valgrind_command}")
	endif()

	set(additional_ctest_args "")
	if (VCPKG_TARGET_IS_LINUX AND (THREAD_SANITIZER OR MEMCHECK_ENABLED OR HELGRIND_ENABLED OR CALLGRIND_ENABLED))
		message(STATUS "Skipping tests that use model runner (Thread Sanitizer or Memcheck or Helgrind or Callgrind) on Linux")
		set(additional_ctest_args --label-exclude uses_model_runner)
	endif()

	vcpkg_execute_required_process(
		COMMAND ${valgrind_command} ${CMAKE_CTEST_COMMAND}
			-V
			--no-tests=error
			--label-regex is_api_test
			${additional_ctest_args}
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-api-${PORT}-${triplet_build_type}
	)

	vcpkg_execute_required_process(
		COMMAND ${valgrind_command} ${CMAKE_CTEST_COMMAND}
			-V
			--no-tests=error
			--label-regex is_example
			${additional_ctest_args}
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-examples-${PORT}-${triplet_build_type}
	)
endfunction()

function(run_all_tests)
	if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL debug)
		run_tests(dbg)
	endif()
	if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL release)
		run_tests(rel)
	endif()
endfunction()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS_NO_CMAKE
	FEATURES
		tests TESTS_ENABLED
		memcheck MEMCHECK_ENABLED
		callgrind CALLGRIND_ENABLED
)

if (TESTS_ENABLED)
	run_all_tests()
endif()
