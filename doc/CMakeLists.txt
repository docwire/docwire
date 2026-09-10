cmake_minimum_required(VERSION 3.16)

# -----------------------------------------------------
# Find doxygen executable
# -----------------------------------------------------

if(DEFINED CUSTOM_DOXYGEN_EXECUTABLE)
    set(DOXYGEN_EXECUTABLE "${CUSTOM_DOXYGEN_EXECUTABLE}"
        CACHE FILEPATH "Path to Doxygen executable" FORCE)
endif()

find_package(Doxygen 1.18.0 REQUIRED)

# -----------------------------------------------------
# Set some of the doxygen configs
# -----------------------------------------------------

set(DOXYGEN_GENERATE_MAN NO)
set(DOXYGEN_GENERATE_HTML YES)

set(DOXYGEN_PROJECT_NAME "DocWire SDK")
set(DOXYGEN_PROJECT_NUMBER ${DOCWIRE_VERSION})
set(DOXYGEN_PROJECT_BRIEF "DocWire SDK: Deterministic, auditable, and secure data processing in C++20. On-premise by default. No compromises on computational speed.")
set(DOXYGEN_STRIP_FROM_PATH ../src/)
set(DOXYGEN_GENERATE_LATEX NO) # Added for consistency. It is set to NO by default by the CMake module.
set(DOXYGEN_EXAMPLE_PATH ../tests/)
set(DOXYGEN_AUTOLINK_SUPPORT YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE YES)
set(DOXYGEN_IMAGE_PATH images)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_BINARY_DIR}/readme.md)

# -----------------------------------------------------
# Create Doxygen pages for Manifesto and Coding Guidelines
# -----------------------------------------------------
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/manifesto.md
     DOXYGEN_MANIFESTO_MD)
string(PREPEND DOXYGEN_MANIFESTO_MD "\\page manifesto Manifesto\n\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/manifesto.md
     "${DOXYGEN_MANIFESTO_MD}")

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/coding_guidelines.md
     DOXYGEN_CODING_GUIDELINES_MD)
string(PREPEND DOXYGEN_CODING_GUIDELINES_MD
       "\\page coding_guidelines Coding Guidelines\n\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/coding_guidelines.md
     "${DOXYGEN_CODING_GUIDELINES_MD}")

# -----------------------------------------------------
# Generate 'doxygen' target
# -----------------------------------------------------

include(../tools/generate_3rdparty_components_doc.cmake)

file(GLOB HEADERS "../src/*.h")

file(READ ../README.md readme_contents)
string(REPLACE "https://docwire.readthedocs.io/en/latest/" "" readme_contents "${readme_contents}")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/readme.md ${readme_contents})

doxygen_add_docs(
    doxygen
    ${CMAKE_CURRENT_BINARY_DIR}/readme.md
    ${CMAKE_CURRENT_BINARY_DIR}/manifesto.md
    ${CMAKE_CURRENT_BINARY_DIR}/coding_guidelines.md
    ${HEADERS}
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
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md DESTINATION share/docwire/doc)
