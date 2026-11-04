add_library(df0_ocr SHARED ocr_parser.cpp)

if(MSVC)
    set_property(TARGET df0_ocr PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")    
endif()

find_package(Leptonica)
find_package(Tesseract)
target_link_libraries(df0_ocr PRIVATE ${Leptonica_LIBRARIES} Tesseract::libtesseract df0_core)

install(TARGETS df0_ocr EXPORT df0_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:df0_ocr> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_ocr EXPORT_FILE_NAME ocr_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ocr_export.h DESTINATION include/docwire)

df0_find_resource(TESSDATA_FULL_PATH REL_PATH "tessdata-fast" REQUIRED)
df0_target_resources(df0_ocr "tessdata-fast" SOURCE "${TESSDATA_FULL_PATH}")
