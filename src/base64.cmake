add_library(docwire_base64 SHARED base64.cpp)

target_compile_features(docwire_base64 PUBLIC cxx_std_20)
if(MSVC)
    add_definitions(-DMSVC_BUILD)
    target_compile_options(docwire_base64 PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

find_library(base64 base64 REQUIRED)
target_link_libraries(docwire_base64 PRIVATE ${base64})
find_path(base64_inc_dir libbase64.h REQUIRED)
target_include_directories(docwire_base64 PRIVATE ${base64_inc_dir})

install(TARGETS docwire_base64)
if(MSVC)
	install(FILES $<TARGET_PDB_FILE:docwire_base64> DESTINATION bin CONFIGURATIONS Debug)
endif()
