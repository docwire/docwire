add_library(df0_openai SHARED
    analyze_data.cpp
    chat.cpp
    classify.cpp
    detect_sentiment.cpp
    embed.cpp
    extract_entities.cpp
    extract_keywords.cpp
    find.cpp
    summarize.cpp
    text_to_speech.cpp
    transcribe.cpp
    translate_to.cpp)

find_package(Boost REQUIRED COMPONENTS json)
target_link_libraries(df0_openai PRIVATE df0_base64 df0_core df0_ai df0_http Boost::json)

install(TARGETS df0_openai EXPORT df0_targets)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:df0_openai> DESTINATION bin CONFIGURATIONS Debug)
endif()

include(GenerateExportHeader)
generate_export_header(df0_openai EXPORT_FILE_NAME openai_export.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/openai_export.h DESTINATION include/df0)
