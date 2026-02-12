add_library(docwire_local_ai SHARED local_ai_embed.cpp model_chain_element.cpp model_runner.cpp tokenizer.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(ctranslate2 CONFIG REQUIRED)
find_library(sentencepiece_LIBRARIES sentencepiece REQUIRED)
if(MSVC)
    find_package(absl CONFIG REQUIRED)
    list(APPEND sentencepiece_LIBRARIES
        absl::strings
        absl::flags
        absl::flags_parse
        absl::log
        absl::check)
    find_package(protobuf CONFIG REQUIRED)
    list(APPEND sentencepiece_LIBRARIES protobuf::libprotobuf-lite)
endif()
target_link_libraries(docwire_local_ai PRIVATE docwire_core docwire_ai Boost::filesystem Boost::json CTranslate2::ctranslate2 ${sentencepiece_LIBRARIES})

install(TARGETS docwire_local_ai EXPORT docwire_targets)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_local_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(docwire_local_ai EXPORT_FILE_NAME local_ai_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/local_ai_export.h DESTINATION include/docwire)

docwire_find_resource(FLAN_T5_FULL_PATH TYPE DIRECTORY REL_PATH "flan-t5-large-ct2-int8" REQUIRED)
docwire_target_resources(docwire_local_ai "flan-t5-large-ct2-int8" SOURCE "${FLAN_T5_FULL_PATH}")

docwire_find_resource(E5_MODEL_FULL_PATH TYPE DIRECTORY REL_PATH "multilingual-e5-small-ct2-int8" REQUIRED)
docwire_target_resources(docwire_local_ai "multilingual-e5-small-ct2-int8" SOURCE "${E5_MODEL_FULL_PATH}")
