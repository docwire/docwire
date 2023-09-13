cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# Get doctotext version from ChangeLog and store it in DOCTOTEXT_VERSION and SIMPLE_DOCTOTEXT_VERSION
function(doctotext_extract_version)
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
	set(DOCTOTEXT_VERSION ${doc_ver} PARENT_SCOPE)
	set(SIMPLE_DOCTOTEXT_VERSION ${simple_doc_ver} PARENT_SCOPE)
endfunction()

doctotext_extract_version()

project(doctotext VERSION ${SIMPLE_DOCTOTEXT_VERSION} LANGUAGES CXX C)
message(STATUS "Build doctotext: ${DOCTOTEXT_VERSION}")

set(CMAKE_CXX_STANDARD 17)

option(DOCTOTEXT_SHARED "Build doctotext as shared library" ON)
option(DOCTOTEXT_DOC "Compile Documentation" ON)
option(DOCTOTEXT_TRACE "Enable Tracing" OFF)

enable_testing ()

file(WRITE ${CMAKE_SOURCE_DIR}/version.h "#define VERSION \"${DOCTOTEXT_VERSION}\"")
file(WRITE ${CMAKE_BINARY_DIR}/VERSION "${DOCTOTEXT_VERSION}")
install(FILES ${CMAKE_SOURCE_DIR}/ChangeLog ${CMAKE_BINARY_DIR}/VERSION DESTINATION share/doctotext)

add_subdirectory(src)

file( GLOB extras extras/* )
foreach(extra ${extras})
   add_subdirectory(${extra})
endforeach()
add_subdirectory(examples)
add_subdirectory(tests)

if (DOCTOTEXT_DOC)
   add_subdirectory(doc)
endif()
