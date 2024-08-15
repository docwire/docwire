set(CMAKE_INSTALL_RPATH .)

set(EMPTY_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp)
file(GENERATE OUTPUT ${EMPTY_SOURCE} CONTENT "
	#ifdef MSVC_BUILD
		#define DllExport  __declspec( dllexport )
	#else
		#define DllExport
	#endif
	namespace docwire
	{
		DllExport void dummy() {}
	}
")

include(core.cmake)
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
include(local_ai.cmake)
include(fuzzy_match.cmake)
include(cli.cmake)

file(GLOB HEADERS "*.h")
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
