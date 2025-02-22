add_library(docwire_ocr SHARED ocr_parser.cpp)

if(MSVC)
    set_property(TARGET docwire_ocr PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")    
endif()

find_package(Leptonica)
find_package(Tesseract)
target_link_libraries(docwire_ocr PRIVATE ${Leptonica_LIBRARIES} Tesseract::libtesseract docwire_core)

install(TARGETS docwire_ocr)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_ocr> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_ocr EXPORT_FILE_NAME ocr_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ocr_export.h DESTINATION include/docwire)
