cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# -----------------------------------------------------
# Start doctotext project
# -----------------------------------------------------

include(cmake/utils.cmake)

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
