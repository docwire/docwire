add_library(docwire_audit SHARED
    audit/cerr_redirection_mutex.cpp)

target_compile_features(docwire_audit PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(docwire_audit PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

target_include_directories(docwire_audit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS docwire_audit EXPORT docwire_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_audit> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_audit EXPORT_FILE_NAME audit_export.h)
target_include_directories(docwire_audit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
# Install audit_export.h alongside the other audit headers so that
# `#include "audit_export.h"` resolves for both in-tree and installed consumers.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/audit_export.h DESTINATION include/docwire/audit)

# Link modules that consume the compiled audit support (e.g. doc_parser's stderr redirection).
# This is intentionally PRIVATE because the dependency does not leak through public headers.
if(TARGET docwire_ole_office_formats)
    target_link_libraries(docwire_ole_office_formats PRIVATE docwire_audit)
endif()
