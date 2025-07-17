add_library(docwire_local_ai SHARED model_chain_element.cpp model_runner.cpp tokenizer.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(ctranslate2 CONFIG REQUIRED)
find_library(sentencepiece_LIBRARIES sentencepiece REQUIRED)
target_link_libraries(docwire_local_ai PRIVATE docwire_core Boost::filesystem Boost::json CTranslate2::ctranslate2 ${sentencepiece_LIBRARIES})

install(TARGETS docwire_local_ai EXPORT docwire_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_local_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_local_ai EXPORT_FILE_NAME local_ai_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/local_ai_export.h DESTINATION include/docwire)
