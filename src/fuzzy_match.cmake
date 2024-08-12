add_library(docwire_fuzzy_match SHARED fuzzy_match.cpp)

find_package(rapidfuzz REQUIRED)
target_link_libraries(docwire_fuzzy_match PRIVATE rapidfuzz::rapidfuzz)

install(TARGETS docwire_fuzzy_match)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_fuzzy_match> DESTINATION bin CONFIGURATIONS Debug)
endif()
