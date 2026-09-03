if(DEFINED ENV{SOURCE_PATH})
	set(SOURCE_PATH $ENV{SOURCE_PATH})
else()
	vcpkg_from_github(
		OUT_SOURCE_PATH SOURCE_PATH
		REPO docwire/docwire
		HEAD_REF master
	)
endif()

# Doxygen version used by documentation builds
set(DOXYGEN_VERSION "1.18.0")

# Doxygen GitHub release tag: "Release_1_18_0"
string(REPLACE "." "_" DOXYGEN_RELEASE_TAG_SUFFIX "${DOXYGEN_VERSION}")
set(DOXYGEN_RELEASE_TAG "Release_${DOXYGEN_RELEASE_TAG_SUFFIX}")

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
	FEATURES
		asan ADDRESS_SANITIZER
		tsan THREAD_SANITIZER
		helgrind HELGRIND_ENABLED
		local-ai-ct2 DOCWIRE_CT2
        local-ai-llama DOCWIRE_LLAMA
        docs DOCWIRE_DOC
)

if(DEFINED ENV{CMAKE_MESSAGE_LOG_LEVEL})
	list(APPEND FEATURE_OPTIONS "-DCMAKE_MESSAGE_LOG_LEVEL=$ENV{CMAKE_MESSAGE_LOG_LEVEL}")
endif()

if(DOCWIRE_DOC)
    if(VCPKG_HOST_IS_WINDOWS)
        set(DOXYGEN_ASSET "doxygen-${DOXYGEN_VERSION}.windows.x64.bin.zip")
        set(DOXYGEN_SHA512 "c4f7b45a4ae5f49b9d232036ec200033be7a44c41dfa717f121c05a06a9377838795ef9e931267dcb4d73be2d3bb34c97d79f46f464c8a4d62ef4664210e5491")
        set(DOXYGEN_EXE_SUFFIX ".exe")
    elseif(VCPKG_HOST_IS_OSX)
        vcpkg_execute_in_download_mode(
            COMMAND uname -m
            OUTPUT_VARIABLE _host_arch
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(DOXYGEN_EXE_SUFFIX "")
        if(_host_arch STREQUAL "arm64")
            set(DOXYGEN_ASSET "doxygen-${DOXYGEN_VERSION}-mac-arm.zip")
            set(DOXYGEN_SHA512 "f56f65f46bb8b8e184677997f63ca6fabcc80da3339e28de7249bac5ce851fd91171447037e1750ef952e5192036fc74593eb57d3c0b39a656624105d63ed530")
        else()
            set(DOXYGEN_ASSET "doxygen-${DOXYGEN_VERSION}-mac-intel.zip")
            set(DOXYGEN_SHA512 "70fb5e7c55681193d9a5f27e684e61187c3af6b793dd2527c20fec23548de1540d0f8a9045f62f161c68853879b3a6233a1d372c5938d505cf3edd035b5f77a1")
        endif()
    else()
        set(DOXYGEN_ASSET "doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz")
        set(DOXYGEN_SHA512 "fd8a26b73dbb29e18ad1966ea44ce09409e23ea6a91b218dcb65d36fb118fcc385ef5b92be93856a54e2746630a2c4c9a4940d0df940e494f918d2a290ed8f96")
        set(DOXYGEN_EXE_SUFFIX "")
    endif()

    vcpkg_download_distfile(
        DOXYGEN_ARCHIVE
        URLS "https://github.com/doxygen/doxygen/releases/download/${DOXYGEN_RELEASE_TAG}/${DOXYGEN_ASSET}"
        FILENAME "${DOXYGEN_ASSET}"
        SHA512 "${DOXYGEN_SHA512}"
    )

    set(DOXYGEN_EXTRACT_DIR "${CURRENT_BUILDTREES_DIR}/doxygen-${DOXYGEN_VERSION}")
    file(REMOVE_RECURSE "${DOXYGEN_EXTRACT_DIR}")
    file(ARCHIVE_EXTRACT
        INPUT "${DOXYGEN_ARCHIVE}"
        DESTINATION "${DOXYGEN_EXTRACT_DIR}"
    )

    file(GLOB_RECURSE DOXYGEN_EXECUTABLE
        "${DOXYGEN_EXTRACT_DIR}/doxygen${DOXYGEN_EXE_SUFFIX}"
    )
    list(GET DOXYGEN_EXECUTABLE 0 DOXYGEN_EXECUTABLE)


    list(APPEND FEATURE_OPTIONS
        "-DCUSTOM_DOXYGEN_EXECUTABLE=${DOXYGEN_EXECUTABLE}"
    )
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
vcpkg_install_copyright(FILE_LIST ${SOURCE_PATH}/LICENSE ${SOURCE_PATH}/doc/COPYING.AGPLv3)

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
