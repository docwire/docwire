cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# Get version from ChangeLog and store it in DOCWIRE_VERSION and SIMPLE_DOCWIRE_VERSION
function(extract_version)
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog" changelog_text)
	if(${changelog_text} MATCHES "Version ([0-9]+\\.[0-9]+\\.[0-9]+):")
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

set(CMAKE_CXX_STANDARD 17)

option(DOCWIRE_DOC "Compile Documentation" ON)
option(DOCWIRE_TRACE "Enable Tracing" OFF)
option(ADDRESS_SANITIZER "Enable address sanitizer" OFF)

if (ADDRESS_SANITIZER)
	message(STATUS "Address sanitizer enabled")
	add_compile_options(-fsanitize=address)
	add_link_options(-fsanitize=address)
endif()

if (THREAD_SANITIZER)
	message(STATUS "Thread sanitizer enabled")
	add_compile_options(-fsanitize=thread)
	add_link_options(-fsanitize=thread)
endif()

enable_testing ()

file(WRITE ${CMAKE_SOURCE_DIR}/version.h "#define VERSION \"${DOCWIRE_VERSION}\"")
file(WRITE ${CMAKE_BINARY_DIR}/VERSION "${DOCWIRE_VERSION}")
install(FILES ${CMAKE_SOURCE_DIR}/ChangeLog ${CMAKE_BINARY_DIR}/VERSION DESTINATION share/docwire)

add_subdirectory(src)

file( GLOB extras extras/* )
foreach(extra ${extras})
   add_subdirectory(${extra})
endforeach()

add_subdirectory(tests)

if (DOCWIRE_DOC)
   add_subdirectory(doc)
endif()
