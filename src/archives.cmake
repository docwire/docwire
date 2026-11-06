add_library(df0_archives SHARED archives_parser.cpp)

find_package(LibArchive REQUIRED)
target_link_libraries(df0_archives PRIVATE LibArchive::LibArchive df0_core)

install(TARGETS df0_archives EXPORT df0_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:df0_archives> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_archives EXPORT_FILE_NAME archives_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/archives_export.h DESTINATION include/df0)
