cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for VSCode CLangd extension

# Get version from ChangeLog and store it in DOCWIRE_VERSION and SIMPLE_DOCWIRE_VERSION
function(extract_version)
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/doc/ChangeLog" changelog_text)
	if(${changelog_text} MATCHES "Version ([0-9]+\\.[0-9]+\\.[0-9]+)")
		set(simple_doc_ver ${CMAKE_MATCH_1})
	else()
		message(FATAL_ERROR "Could not extract version number from ChangeLog")
	endif()
	if (changelog_text MATCHES "Unreleased")
		set(doc_ver ${simple_doc_ver}-dirty)
	else()
		set(doc_ver ${simple_doc_ver})
	endif()
	set(DOCWIRE_VERSION ${doc_ver} PARENT_SCOPE)
	set(SIMPLE_DOCWIRE_VERSION ${simple_doc_ver} PARENT_SCOPE)
endfunction()

extract_version()

project(docwire VERSION ${SIMPLE_DOCWIRE_VERSION} LANGUAGES CXX C)
message(STATUS "Build docwire: ${DOCWIRE_VERSION}")

option(DOCWIRE_DOC "Compile Documentation" ON)
option(DOCWIRE_TRACE "Enable Tracing" OFF)
option(ADDRESS_SANITIZER "Enable address sanitizer" OFF)

if (ADDRESS_SANITIZER)
	if (NOT MSVC OR CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.34") # 19.34 is Visual Studio 2022 17.4
		message(STATUS "Address sanitizer enabled")
		add_compile_options(-fsanitize=address)
		add_link_options(-fsanitize=address)
		message(STATUS "ASAN_OPTIONS environment variable: $ENV{ASAN_OPTIONS}")
	elseif (MSVC)
		message(WARNING "Address sanitizer is not enabled in release mode for MSVC version less than 17.4 due to known issues")
	endif()
endif()

if (THREAD_SANITIZER)
	message(STATUS "Thread sanitizer enabled")
	add_compile_options(-fsanitize=thread)
	add_link_options(-fsanitize=thread)
endif()

enable_testing ()

file(WRITE ${CMAKE_SOURCE_DIR}/version.h "#define VERSION \"${DOCWIRE_VERSION}\"")
file(WRITE ${CMAKE_BINARY_DIR}/VERSION "${DOCWIRE_VERSION}")
install(FILES ${CMAKE_BINARY_DIR}/VERSION DESTINATION share/docwire)

string(TOLOWER ${CMAKE_CXX_COMPILER_ID} compiler_id)
if(compiler_id STREQUAL "gnu")
    set(compiler_id "gcc")
endif()
file(WRITE ${CMAKE_BINARY_DIR}/abi-id.txt "${compiler_id}-${CMAKE_CXX_COMPILER_VERSION}")
install(FILES ${CMAKE_BINARY_DIR}/abi-id.txt DESTINATION share/docwire)

add_subdirectory(src)

file( GLOB extras extras/* )
foreach(extra ${extras})
   add_subdirectory(${extra})
endforeach()

add_subdirectory(tests)

if (DOCWIRE_DOC)
   add_subdirectory(doc)
endif()
