set(CMAKE_CXX_STANDARD 17)

find_package(GTest CONFIG REQUIRED)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE doctotext_core GTest::gtest_main)
if(MSVC)
    set_target_properties( api_tests
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
            )
    find_package(PThreads4W REQUIRED)
    target_link_libraries(api_tests PRIVATE PThreads4W::PThreads4W)
endif()

add_executable(c_api_tests c_api_tests.cpp)
target_link_libraries(c_api_tests PRIVATE doctotext_core GTest::gtest_main)

if(MSVC)
    set_target_properties( c_api_tests
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
            )
endif()

include(GoogleTest)

gtest_discover_tests(api_tests
        DISCOVERY_MODE PRE_TEST
        DISCOVERY_TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

gtest_discover_tests(c_api_tests
        DISCOVERY_MODE PRE_TEST
        DISCOVERY_TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
