set(EMPTY_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ai_empty.cpp)
file(GENERATE OUTPUT ${EMPTY_SOURCE} CONTENT "
	#include \"ai_elements.h\"
	namespace docwire::ai
	{
		// This dummy function is required to ensure that the shared library is created.
		DOCWIRE_AI_EXPORT void dummy_function_for_docwire_ai() {}
	}
")
add_library(docwire_ai SHARED ${EMPTY_SOURCE})

# Enable access to SDK headers
target_include_directories(docwire_ai PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # during building headers are in the source tree
    $<INSTALL_INTERFACE:include>) # after installation headers are in include/docwire

install(TARGETS docwire_ai EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_ai EXPORT_FILE_NAME ai_export.h)
target_include_directories(docwire_ai PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ai_export.h DESTINATION include/docwire)