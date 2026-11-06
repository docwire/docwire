add_library(df0_audit SHARED
    audit/cerr_redirection_mutex.cpp)

target_compile_features(df0_audit PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(df0_audit PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

target_include_directories(df0_audit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS df0_audit EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_audit> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_audit EXPORT_FILE_NAME audit_export.h)
target_include_directories(df0_audit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
# Install audit_export.h alongside the other audit headers so that
# `#include "audit_export.h"` resolves for both in-tree and installed consumers.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/audit_export.h DESTINATION include/df0/audit)
