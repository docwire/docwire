set(CMAKE_CXX_STANDARD 17)

find_package(GTest CONFIG REQUIRED)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE doctotext_core GTest::gtest_main)
if(MSVC)
    find_package(PThreads4W REQUIRED)
    target_link_libraries(api_tests PRIVATE PThreads4W::PThreads4W)
endif()

file(GLOB test_files *)
list(FILTER test_files EXCLUDE REGEX ".*\\.cpp$")
file(COPY ${test_files} DESTINATION .)

include(GoogleTest)

gtest_discover_tests(api_tests
        DISCOVERY_MODE PRE_TEST
        DISCOVERY_TIMEOUT 30
)
