set(CMAKE_INSTALL_RPATH .)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_compile_definitions(DOCWIRE_ENABLE_SHORT_MACRO_NAMES)

add_subdirectory(wv2)
include(core.cmake)
include(base64.cmake)
include(odf_ooxml.cmake)
include(ole_office_formats.cmake)
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
include(local_ai.cmake)
include(fuzzy_match.cmake)
include(content_type.cmake)
include(http.cmake)
include(openai.cmake)
include(cli.cmake)

file(GLOB HEADERS "*.h")
list(REMOVE_ITEM HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/misc.h
	${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_ole_storage.h
	${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_ole_stream_reader.h)
install(FILES ${HEADERS} DESTINATION include/docwire)

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
	set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
endif()

include(InstallRequiredSystemLibraries)

if (WIN32)
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin/docwire_system_libraries)
else()
	install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION lib/docwire_system_libraries)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
	docwire-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/docwire-config.cmake
	INSTALL_DESTINATION lib/cmake/docwire
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/docwire-config.cmake DESTINATION lib/cmake/docwire)
install(EXPORT docwire_targets FILE docwire-targets.cmake DESTINATION lib/cmake/docwire)
