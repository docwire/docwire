cmake_minimum_required(VERSION 3.16)

# -----------------------------------------------------
# Find doxygen executable
# -----------------------------------------------------

find_package(Doxygen REQUIRED)
if(NOT DOXYGEN_FOUND)
    message(FATAL ERROR "Could not find the doxygen executable")
endif()

# -----------------------------------------------------
# Set some of the doxygen configs
# -----------------------------------------------------

set(DOXYGEN_GENERATE_MAN NO)
set(DOXYGEN_GENERATE_HTML YES)

set(DOXYGEN_PROJECT_NAME "DocWire SDK")
set(DOXYGEN_PROJECT_NUMBER ${DOCWIRE_VERSION})
set(DOXYGEN_PROJECT_BRIEF "DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing. Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining, document analysis. Offline processing possible for security and confidentiality")
set(DOXYGEN_STRIP_FROM_PATH ../src/)
set(DOXYGEN_GENERATE_LATEX NO) # Added for consistency. It is set to NO by default by the CMake module.
set(DOXYGEN_EXAMPLE_PATH ../examples/)
set(DOXYGEN_AUTOLINK_SUPPORT YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE YES)
set(DOXYGEN_IMAGE_PATH images)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ../README.md)

# -----------------------------------------------------
# Generate 'doxygen' target
# -----------------------------------------------------

include(../tools/generate_3rdparty_components_doc.cmake)

file(GLOB HEADERS "../src/*.h")

doxygen_add_docs(
    doxygen
    ../README.md ${HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/3rdparty_components.md
    COMMENT "Generate html documentation of DocWire SDK"
    ALL
)

configure_file(index.html ${CMAKE_BINARY_DIR}/doc/index.html COPYONLY)

# -----------------------------------------------------
# Generate 'install' target
# -----------------------------------------------------

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/docwire/doc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/index.html DESTINATION share/docwire/doc)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog DESTINATION share/docwire/doc)
