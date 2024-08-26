add_library(docwire_local_ai SHARED model_runner.cpp)

find_package(Boost REQUIRED COMPONENTS filesystem system json)
find_package(ctranslate2 CONFIG REQUIRED)
find_library(opennmt_tokenizer OpenNMTTokenizer REQUIRED)
target_link_libraries(docwire_local_ai PRIVATE docwire_core Boost::filesystem Boost::json CTranslate2::ctranslate2 ${opennmt_tokenizer})

install(TARGETS docwire_local_ai)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:docwire_local_ai> DESTINATION bin CONFIGURATIONS Debug)
endif()
