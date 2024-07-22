find_package(GTest CONFIG REQUIRED)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr GTest::gtest_main)

find_package(Boost REQUIRED COMPONENTS json)
target_link_libraries(api_tests PRIVATE Boost::json)
target_include_directories(api_tests PRIVATE ${Boost_INCLUDE_DIRS})

file(GLOB test_files *)
list(FILTER test_files EXCLUDE REGEX ".*\\.cpp$")
file(COPY ${test_files} DESTINATION .)

include(GoogleTest)

gtest_discover_tests(api_tests
        DISCOVERY_MODE PRE_TEST
        DISCOVERY_TIMEOUT 30
)

file(READ ../README.md content)
set(n 1)
while (TRUE)
	string(FIND "${content}" "```cpp" start_pos)
	if (start_pos EQUAL -1)
		break()
	endif()
	math(EXPR start_pos "${start_pos} + 6")
	string(SUBSTRING "${content}" ${start_pos} -1 content)
	string(FIND "${content}" "```" end_pos)
	if (end_pos EQUAL -1)
		message(FATAL_ERROR "Failed to find end of code block starting at position ${start_pos}")
	endif()
	string(SUBSTRING "${content}" 0 ${end_pos} code_block)
	set(example_file "${CMAKE_BINARY_DIR}/example_${n}.cpp")
	file(WRITE ${example_file} "${code_block}")
	add_executable(example_${n} ${example_file})
	target_include_directories(example_${n} PUBLIC ../src)
	target_link_libraries(example_${n} PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_local_ai)
	string(FIND "${code_block}" "OPENAI_API_KEY" openai_api_key_pos)
	if (openai_api_key_pos EQUAL -1 OR NOT "$ENV{OPENAI_API_KEY}" STREQUAL "")
		add_test(NAME example_${n} COMMAND example_${n})
	endif()
	math(EXPR n "${n} + 1")
	math(EXPR end_pos "${end_pos} + 3")
	string(SUBSTRING "${content}" ${end_pos} -1 content)
endwhile()
