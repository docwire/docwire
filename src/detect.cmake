add_library(docwire_detect SHARED detect_by_signature.cpp)

find_package(unofficial-libmagic REQUIRED)
target_link_libraries(docwire_detect PRIVATE unofficial::libmagic::libmagic docwire_core)

install(TARGETS docwire_detect)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_detect> DESTINATION bin CONFIGURATIONS Debug)
endif()
