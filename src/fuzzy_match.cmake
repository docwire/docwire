add_library(docwire_fuzzy_match SHARED fuzzy_match.cpp)

find_package(rapidfuzz REQUIRED)
target_link_libraries(docwire_fuzzy_match PRIVATE rapidfuzz::rapidfuzz docwire_core)

install(TARGETS docwire_fuzzy_match)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_fuzzy_match> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_fuzzy_match EXPORT_FILE_NAME fuzzy_match_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fuzzy_match_export.h DESTINATION include/docwire)
