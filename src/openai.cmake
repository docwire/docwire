add_library(docwire_openai SHARED
    analyze_data.cpp
    chat.cpp
    classify.cpp
    detect_sentiment.cpp
    extract_entities.cpp
    extract_keywords.cpp
    find.cpp
    summarize.cpp
    text_to_speech.cpp
    transcribe.cpp
    translate_to.cpp)

target_link_libraries(docwire_openai PRIVATE docwire_base64 docwire_core)

install(TARGETS docwire_openai)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_openai> DESTINATION bin CONFIGURATIONS Debug)
endif()
