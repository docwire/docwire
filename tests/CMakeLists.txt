set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.0.zip
        URL_HASH SHA1=b98f4d04dd684c1e47c3f48a6a7fa0d24b98f4ac
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
if(CMAKE_CROSSCOMPILING)
    set(dis_mode "PRE_TEST")
    set(CMAKE_CROSSCOMPILING_EMULATOR "/usr/bin/wine")
else()
    set(dis_mode "POST_BUILD")
endif()
FetchContent_MakeAvailable(googletest)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE doctotext_core gtest_main)
if(MSVC)
    set_target_properties( api_tests
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
            )
    find_library(pthread pthreadVC3 HINTS "C:/vcpkg/packages/pthreads_x64-windows/lib")
    find_library(pthread_d pthreadVC3d HINTS "C:/vcpkg/packages/pthreads_x64-windows/debug/lib")
    target_link_directories(doctotext_core PUBLIC "C:/lib")
    target_link_libraries(api_tests PRIVATE debug ${pthread_d} optimized ${pthread})
endif()
target_compile_options(api_tests PRIVATE -O0 -g)

add_executable(c_api_tests c_api_tests.cpp)
target_link_libraries(c_api_tests PRIVATE doctotext_core gtest_main)
target_compile_options(c_api_tests PRIVATE -O0 -g)

if(MSVC)
    set_target_properties( c_api_tests
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
            )
endif()

include(GoogleTest)

gtest_discover_tests(api_tests
        DISCOVERY_MODE ${dis_mode}
        DISCOVERY_TIMEOUT 30
)

gtest_discover_tests(c_api_tests
        DISCOVERY_MODE ${dis_mode}
        DISCOVERY_TIMEOUT 30
)