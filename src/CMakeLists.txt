set(CMAKE_INSTALL_RPATH .)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_compile_definitions(DF0_ENABLE_SHORT_MACRO_NAMES)

add_subdirectory(wv2)

include(df0_resources.cmake)

include(core.cmake)
include(ole_utils.cmake)
include(base64.cmake)
include(odf_ooxml.cmake)
include(ole_office_formats.cmake)
include(audit.cmake)
include(html.cmake)
include(xml.cmake)
include(plain_text.cmake)
include(iwork.cmake)
include(xlsb.cmake)
include(rtf.cmake)
include(pdf.cmake)
include(office_formats.cmake)
include(ocr.cmake)
include(mail.cmake)
include(archives.cmake)
include(ai.cmake)

if(DF0_CT2)
    include(ai_ct2.cmake)
endif()

if(DF0_LLAMA)
    include(ai_llama.cmake)
endif()

if(DF0_CT2 OR DF0_LLAMA)
    include(local_ai.cmake)
endif()

include(fuzzy_match.cmake)
include(content_type.cmake)

include(http.cmake)
include(openai.cmake)
include(cli.cmake)

file(GLOB HEADERS "*.h" "*.hpp")
install(FILES ${HEADERS} DESTINATION include/df0)

file(GLOB PIPELINE_HEADERS "pipeline/*.hpp")
install(FILES ${PIPELINE_HEADERS} DESTINATION include/df0/pipeline)

file(GLOB PLAIN_TEXT_HEADERS "plain_text/*.h" "plain_text/*.hpp")
install(FILES ${PLAIN_TEXT_HEADERS} DESTINATION include/df0/plain_text)

file(GLOB AUDIT_HEADERS "audit/*.hpp")
install(FILES ${AUDIT_HEADERS} DESTINATION include/df0/audit)

file(GLOB AUDIT_DETAIL_HEADERS "audit/detail/*.hpp")
install(FILES ${AUDIT_DETAIL_HEADERS} DESTINATION include/df0/audit/detail)

file(GLOB AUDIT_SERIALIZE_HEADERS "audit/serialize/*.hpp")
install(FILES ${AUDIT_SERIALIZE_HEADERS} DESTINATION include/df0/audit/serialize)

file(GLOB AUDIT_SINK_HEADERS "audit/sink/*.hpp")
install(FILES ${AUDIT_SINK_HEADERS} DESTINATION include/df0/audit/sink)

include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	df0-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/df0-config.cmake
	INSTALL_DESTINATION lib/cmake/df0
	PATH_VARS CMAKE_INSTALL_DATADIR
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/df0-config.cmake DESTINATION lib/cmake/df0)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/df0_resources.cmake DESTINATION lib/cmake/df0)
install(EXPORT df0_targets FILE df0-targets.cmake DESTINATION lib/cmake/df0)
