add_library(docwire_ole_utils STATIC
    oshared.cpp)

target_link_libraries(docwire_ole_utils
    PUBLIC docwire_wv2
    PRIVATE docwire_core)
