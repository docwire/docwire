if(DEFINED ENV{SOURCES_ARCHIVE})
	vcpkg_extract_source_archive(
		SOURCE_PATH
		ARCHIVE $ENV{SOURCES_ARCHIVE}
		NO_REMOVE_ONE_LEVEL
	)
else()
	vcpkg_from_github(
		OUT_SOURCE_PATH SOURCE_PATH
		REPO docwire/doctotext
		REF de06a84a3abb412cc976b1f61b0613ea901a4e8a
		SHA512 05b793d83d36a62d45bf0bc37a7cedfe8a4b205370f57f79d8c3b93a46001ffb5ebdbe31fac04e69f6439fffe99ff18a2d876faa4e4c5cab3cb14c06e13d726d
		HEAD_REF master
	)
endif()

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/tools)
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/text_extractor" "${CURRENT_PACKAGES_DIR}/tools/text_extractor")
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/text_extractor.sh" "${CURRENT_PACKAGES_DIR}/tools/text_extractor.sh")
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/c_text_extractor" "${CURRENT_PACKAGES_DIR}/tools/c_text_extractor")
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/c_text_extractor.sh" "${CURRENT_PACKAGES_DIR}/tools/c_text_extractor.sh")
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/tools)
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/text_extractor" "${CURRENT_PACKAGES_DIR}/debug/tools/text_extractor")
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/text_extractor.sh" "${CURRENT_PACKAGES_DIR}/debug/tools/text_extractor.sh")
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/c_text_extractor" "${CURRENT_PACKAGES_DIR}/debug/tools/c_text_extractor")
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/c_text_extractor.sh" "${CURRENT_PACKAGES_DIR}/debug/tools/c_text_extractor.sh")
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static" OR NOT VCPKG_TARGET_IS_WINDOWS)
	file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(INSTALL "${SOURCE_PATH}/COPYING.GPL" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)

function(run_tests build_type)
	set(triplet_build_type ${TARGET_TRIPLET}-${build_type})
        message(STATUS "Testing ${triplet_build_type}")
	message(STATUS "Tests working directory: ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}")
	vcpkg_execute_required_process(
		COMMAND "ctest"
			-V
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-${PORT}-${triplet_build_type}
	)
endfunction()

if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL debug)
	set(BACKUP_LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
	set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/debug/lib:${CURRENT_INSTALLED_DIR}/debug/lib")
	run_tests(dbg)
	set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}")
endif()
if(NOT DEFINED VCPKG_BUILD_TYPE OR VCPKG_BUILD_TYPE STREQUAL release)
	set(BACKUP_LD_LIBRARY_PATH $ENV{LD_LIBRARY_PATH})
	set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}:${CURRENT_PACKAGES_DIR}/lib:${CURRENT_INSTALLED_DIR}/lib")
	run_tests(rel)
	set(ENV{LD_LIBRARY_PATH} "${BACKUP_LD_LIBRARY_PATH}")
endif()
