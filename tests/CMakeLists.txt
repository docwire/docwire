find_package(GTest CONFIG REQUIRED)

add_executable(api_tests api_tests.cpp)
if(MSVC)
    target_compile_options(api_tests PRIVATE /bigobj)
endif()
target_link_libraries(api_tests PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_archives
	docwire_fuzzy_match docwire_base64 docwire_content_type docwire_http GTest::gtest GTest::gmock)
target_compile_definitions(api_tests PRIVATE DOCWIRE_ENABLE_SHORT_MACRO_NAMES)

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
		PROPERTIES LABELS "is_api_test"
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
	target_link_libraries(example_${n} PRIVATE docwire_core docwire_office_formats docwire_mail
		docwire_ocr docwire_archives docwire_local_ai docwire_openai docwire_fuzzy_match docwire_content_type
		docwire_http)
	string(FIND "${code_block}" "OPENAI_API_KEY" openai_api_key_pos)
	if (openai_api_key_pos EQUAL -1 OR NOT "$ENV{OPENAI_API_KEY}" STREQUAL "")
		message(STATUS "Adding test example_${n}")
		add_test(NAME example_${n} COMMAND example_${n})
		string(FIND "${code_block}" "model_runner" model_runner_pos)
		if (NOT model_runner_pos EQUAL -1)
			message(STATUS "Labeling test example_${n} as uses_model_runner")
			set_property(TEST example_${n} PROPERTY LABELS "is_example;uses_model_runner")
		else()
			set_property(TEST example_${n} PROPERTY LABELS "is_example")
		endif()
	else()
		message(STATUS "Skipping example_${n} because OPENAI_API_KEY is not set")
	endif()
	math(EXPR n "${n} + 1")
	math(EXPR end_pos "${end_pos} + 3")
	string(SUBSTRING "${content}" ${end_pos} -1 content)
endwhile()

foreach(example IN ITEMS handling_errors_and_warnings file_type_determination)
	message(STATUS "Adding test ${example}")
	add_executable(${example} ${example}.cpp)
	target_include_directories(${example} PUBLIC ../src)
	target_link_libraries(${example} PRIVATE docwire_core docwire_office_formats docwire_mail
		docwire_ocr docwire_archives docwire_local_ai docwire_fuzzy_match docwire_content_type)
	add_test(NAME ${example} COMMAND ${example})
	set_property(TEST ${example} PROPERTY LABELS "is_example")
endforeach()

foreach(file IN ITEMS 1.doc 1.pdf basic_ocr-eng.png)
	add_test(
		NAME cli_${file}
		COMMAND ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/docwire tests/${file}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} # ensure CLI is running correctly outside build directory
	)
	set_property(TEST cli_${file} PROPERTY LABELS "is_after_installation_test")
endforeach()

message(STATUS "Adding integration test" ${VCPKG_TARGET_TRIPLET} ${VCPKG_DEFAULT_TRIPLET})
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/integration_test/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/integration_test
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/integration_example.cmake ${CMAKE_BINARY_DIR}/integration_test/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/integration_example.cpp ${CMAKE_BINARY_DIR}/integration_test/integration_example.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/integration_example.cmake ${CMAKE_CURRENT_SOURCE_DIR}/integration_example.cpp
)
add_custom_target(setup_integration_test ALL
	DEPENDS ${CMAKE_BINARY_DIR}/integration_test/CMakeLists.txt
)
add_test(
	NAME integration_configure_test
	COMMAND ${CMAKE_COMMAND}
		-DCMAKE_TOOLCHAIN_FILE=${VCPKG_INSTALLED_DIR}/../scripts/buildsystems/vcpkg.cmake
		-DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}
		-S ${CMAKE_BINARY_DIR}/integration_test
		-B ${CMAKE_BINARY_DIR}/integration_test_build
)
set_property(TEST integration_configure_test PROPERTY LABELS is_after_installation_test)
add_test(
	NAME integration_build_test
	COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/integration_test_build
)
set_property(TEST integration_build_test PROPERTY LABELS is_after_installation_test)
set_property(TEST integration_build_test PROPERTY DEPENDS integration_configure_test)
add_test(
	NAME integration_run_test
	COMMAND integration_example
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/integration_test_build
)
set_property(TEST integration_run_test PROPERTY LABELS is_after_installation_test)
set_property(TEST integration_run_test PROPERTY DEPENDS integration_build_test)
