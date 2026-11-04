
message(STATUS "DF0_CT2 enabled: Building CT2 backend.")

add_library(df0_ai_ct2 SHARED ct2_runner.cpp tokenizer.cpp)

target_compile_definitions(df0_ai_ct2 PUBLIC DF0_CT2)

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

target_link_libraries(df0_ai_ct2 PRIVATE df0_core df0_ai Boost::filesystem Boost::system Boost::json CTranslate2::ctranslate2 ${sentencepiece_LIBRARIES})

df0_find_resource(FLAN_T5_FULL_PATH REL_PATH "flan-t5-large-ct2-int8" REQUIRED)
df0_target_resources(df0_ai_ct2 "flan-t5-large-ct2-int8" SOURCE "${FLAN_T5_FULL_PATH}")

df0_find_resource(E5_MODEL_FULL_PATH REL_PATH "multilingual-e5-small-ct2-int8" REQUIRED)
df0_target_resources(df0_ai_ct2 "multilingual-e5-small-ct2-int8" SOURCE "${E5_MODEL_FULL_PATH}")

if(MSVC)
    install(FILES $<TARGET_PDB_FILE:df0_ai_ct2> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)

generate_export_header(df0_ai_ct2 EXPORT_FILE_NAME ai_ct2_export.h)

target_include_directories(df0_ai_ct2 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/docwire>
)

install(TARGETS df0_ai_ct2 EXPORT df0_targets)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ai_ct2_export.h
    DESTINATION include/docwire
)
