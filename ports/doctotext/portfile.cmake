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
		REF 0c44b01cc42adca158b3f01e59a49e49e551f49d
		SHA512 046dbbb1c48be9a068a1b40f226b5f2140100c09542f6753953425eb5033dece002461d11e80334324ee107d5928da0f86112711697afec28964857c11e044e8
		HEAD_REF master
	)
endif()

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
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
	"${CURRENT_PACKAGES_DIR}/bin/text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/tools/text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
file(RENAME
	"${CURRENT_PACKAGES_DIR}/bin/c_text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/tools/c_text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/text_extractor${script_suffix}" "${CURRENT_PACKAGES_DIR}/tools/text_extractor${script_suffix}")
file(RENAME "${CURRENT_PACKAGES_DIR}/bin/c_text_extractor${script_suffix}" "${CURRENT_PACKAGES_DIR}/tools/c_text_extractor${script_suffix}")
file(MAKE_DIRECTORY ${CURRENT_PACKAGES_DIR}/debug/tools)
file(RENAME
	"${CURRENT_PACKAGES_DIR}/debug/bin/text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/debug/tools/text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
file(RENAME
	"${CURRENT_PACKAGES_DIR}/debug/bin/c_text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
	"${CURRENT_PACKAGES_DIR}/debug/tools/c_text_extractor${VCPKG_TARGET_EXECUTABLE_SUFFIX}"
)
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/text_extractor${script_suffix}" "${CURRENT_PACKAGES_DIR}/debug/tools/text_extractor${script_suffix}")
file(RENAME "${CURRENT_PACKAGES_DIR}/debug/bin/c_text_extractor${script_suffix}" "${CURRENT_PACKAGES_DIR}/debug/tools/c_text_extractor${script_suffix}")
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static" OR NOT VCPKG_TARGET_IS_WINDOWS)
	file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(INSTALL "${SOURCE_PATH}/COPYING.GPL" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)

function(run_tests build_type)
	set(triplet_build_type ${TARGET_TRIPLET}-${build_type})
	message(STATUS "Testing ${triplet_build_type}")
	vcpkg_execute_required_process(
		COMMAND "ctest"
			-V
		WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}/${triplet_build_type}
		LOGNAME test-${PORT}-${triplet_build_type}
	)
endfunction()

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
