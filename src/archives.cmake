add_library(docwire_archives SHARED archives_parser.cpp)

find_package(LibArchive REQUIRED)
target_link_libraries(docwire_archives PRIVATE LibArchive::LibArchive docwire_core)

install(TARGETS docwire_archives EXPORT docwire_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_archives> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_archives EXPORT_FILE_NAME archives_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/archives_export.h DESTINATION include/docwire)
