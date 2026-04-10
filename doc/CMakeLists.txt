cmake_minimum_required(VERSION 3.16)

find_package(Doxygen REQUIRED)
if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Could not find the doxygen executable")
endif()

# Configuration
set(DOXYGEN_PROJECT_NAME "DocWire SDK")
set(DOXYGEN_PROJECT_NUMBER ${DOCWIRE_VERSION})
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_MAN NO)
set(DOXYGEN_GENERATE_LATEX NO)

# Force absolute path for the custom header
get_filename_component(ACTUAL_HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/header.html" ABSOLUTE)
set(DOXYGEN_HTML_HEADER "${ACTUAL_HEADER_PATH}")

set(DOXYGEN_PROJECT_BRIEF "DocWire SDK: Award-winning modern data processing in C++20...")
set(DOXYGEN_STRIP_FROM_PATH ../src/)
set(DOXYGEN_EXAMPLE_PATH ../tests/)
set(DOXYGEN_AUTOLINK_SUPPORT YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE YES)
set(DOXYGEN_IMAGE_PATH images)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_BINARY_DIR}/readme.md)

# Build Target logic
include(../tools/generate_3rdparty_components_doc.cmake)
file(GLOB HEADERS "../src/*.h")
file(READ ../README.md readme_contents)
string(REPLACE "https://docwire.readthedocs.io/en/latest/" "" readme_contents "${readme_contents}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/readme.md ${readme_contents})

doxygen_add_docs(
    doxygen
    ${CMAKE_CURRENT_BINARY_DIR}/readme.md
    ${HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/3rdparty_components.md
    COMMENT "Generate html documentation of DocWire SDK"
    ALL
)

configure_file(index.html ${CMAKE_BINARY_DIR}/doc/index.html COPYONLY)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/docwire/doc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/index.html DESTINATION share/docwire/doc)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md DESTINATION share/docwire/doc)
