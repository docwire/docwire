add_library(df0_fuzzy_match SHARED fuzzy_match.cpp)

find_package(rapidfuzz REQUIRED)
target_link_libraries(df0_fuzzy_match PRIVATE rapidfuzz::rapidfuzz df0_core)

install(TARGETS df0_fuzzy_match EXPORT df0_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:df0_fuzzy_match> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_fuzzy_match EXPORT_FILE_NAME fuzzy_match_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fuzzy_match_export.h DESTINATION include/df0)
