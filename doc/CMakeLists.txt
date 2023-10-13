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
set(DOXYGEN_PROJECT_NUMBER ${DOCTOTEXT_VERSION})
set(DOXYGEN_PROJECT_BRIEF "A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html. Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing. To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications. It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition. This document parser is able to extract metadata along with annotations and supports a list of formats that include: DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT), PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)")
set(DOXYGEN_STRIP_FROM_PATH ../src/)
set(DOXYGEN_GENERATE_LATEX NO) # Added for consistency. It is set to NO by default by the CMake module.
set(DOXYGEN_EXAMPLE_PATH ../examples/)
set(DOXYGEN_AUTOLINK_SUPPORT YES)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_REFERENCES_LINK_SOURCE YES)
set(DOXYGEN_IMAGE_PATH images)
set(DOXYGEN_GENERATE_TREEVIEW YES)

# -----------------------------------------------------
# Generate 'doxygen' target
# -----------------------------------------------------

doxygen_add_docs(
    doxygen
    ./mainpage.dox api.dox plugins.dox text_extractor.dox ../src/importer.h ../src/exporter.h ../src/transformer_func.h ../src/parsing_chain.h ../src/html_writer.h ../src/meta_data_writer.h ../src/parser.h ../src/parser_builder.h ../src/parser_manager.h  ../src/parser_parameters.h ../src/parser_wrapper.h ../src/plain_text_writer.h ../src/simple_extractor.h ../src/standard_filter.h ../src/metadata.h ../src/link.h ../src/formatting_style.h ../src/exception.h ../src/parser_provider.h
    ../examples/example_1.cpp ../examples/example_2.cpp ../examples/example_3.cpp ../examples/example_4.cpp ../examples/example_5.cpp ../examples/example_6.cpp ../examples/example_7.cpp ../examples/example_8.cpp ../examples/example_9.cpp
    COMMENT "Generate html documentation of DocWire SDK"
    ALL
)

configure_file(index.html ${CMAKE_BINARY_DIR}/doc/index.html COPYONLY)

# -----------------------------------------------------
# Generate 'install' target
# -----------------------------------------------------

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/docwire/doc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/index.html DESTINATION share/docwire/doc)
