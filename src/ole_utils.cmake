add_library(df0_ole_utils STATIC
    oshared.cpp)

target_link_libraries(df0_ole_utils
    PUBLIC df0_wv2
    PRIVATE df0_core)
