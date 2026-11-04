add_library(df0_pdf SHARED pdf_parser.cpp)

find_library(pdfium pdfium)
find_package(Leptonica)
target_link_libraries(df0_pdf PRIVATE ${pdfium} ${Leptonica_LIBRARIES} df0_core)

install(TARGETS df0_pdf EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_pdf> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_pdf EXPORT_FILE_NAME pdf_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pdf_export.h DESTINATION include/docwire)
