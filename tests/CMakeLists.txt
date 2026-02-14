find_package(GTest CONFIG REQUIRED)

add_executable(api_tests api_tests.cpp)
target_link_libraries(api_tests PRIVATE docwire_core docwire_office_formats docwire_mail docwire_ocr docwire_archives
	docwire_fuzzy_match docwire_base64 docwire_content_type docwire_http docwire_local_ai docwire_openai
	GTest::gtest GTest::gmock)
target_compile_definitions(api_tests PRIVATE DOCWIRE_ENABLE_SHORT_MACRO_NAMES)

find_package(Boost REQUIRED COMPONENTS json)
target_link_libraries(api_tests PRIVATE Boost::json)
target_include_directories(api_tests PRIVATE ${Boost_INCLUDE_DIRS})
docwire_deploy_resources(api_tests)

if(WIN32)
	message(VERBOSE "CMAKE_PREFIX_PATH for tests: ${CMAKE_PREFIX_PATH}")
	set(docwire_test_path_dirs "$<TARGET_FILE_DIR:docwire_core>")
	# Add bin directories from CMAKE_PREFIX_PATH to find external DLLs (e.g. provided by vcpkg)
	foreach(prefix IN LISTS CMAKE_PREFIX_PATH)
		list(APPEND docwire_test_path_dirs "$<$<CONFIG:Debug>:${prefix}/debug/bin>")
		list(APPEND docwire_test_path_dirs "$<$<NOT:$<CONFIG:Debug>>:${prefix}/bin>")
	endforeach()
	list(REMOVE_DUPLICATES docwire_test_path_dirs)
	# Escape semicolons in the host PATH so they aren't interpreted as list separators by CMake
	string(REPLACE ";" "\;" escaped_path "$ENV{PATH}")
	# Join with escaped semicolons for the ENVIRONMENT property
	set(docwire_test_env_path "PATH=$<JOIN:${docwire_test_path_dirs},\;>\;${escaped_path}")
	message(VERBOSE "docwire_test_env_path: ${docwire_test_env_path}")
endif()

add_executable(xml_tests xml_tests.cpp)
target_link_libraries(xml_tests PRIVATE docwire_core docwire_xml GTest::gtest GTest::gmock)
target_compile_definitions(xml_tests PRIVATE DOCWIRE_ENABLE_SHORT_MACRO_NAMES)
docwire_deploy_resources(xml_tests)

file(GLOB test_files *)
list(FILTER test_files EXCLUDE REGEX ".*\\.cpp$")
file(COPY ${test_files} DESTINATION .)

include(GoogleTest)

# Run all gtests within a single process. This is much faster, especially under Valgrind,
# as it avoids the overhead of starting a new process for each of the 700+ tests.
add_test(NAME api_tests COMMAND api_tests)
set_property(TEST api_tests PROPERTY LABELS "is_api_test")
if(WIN32)
	set_property(TEST api_tests APPEND PROPERTY ENVIRONMENT "${docwire_test_env_path}")
endif()

add_test(NAME xml_tests COMMAND xml_tests)
set_property(TEST xml_tests PROPERTY LABELS "is_api_test")
if(WIN32)
	set_property(TEST xml_tests APPEND PROPERTY ENVIRONMENT "${docwire_test_env_path}")
endif()

# This script verifies that snippets in README.md are consistent with their corresponding full example source files
# and that the README structure is correct. It works by:
# 1. Iterating through the README.md content.
# 2. Finding all ```cpp code blocks and ensuring each is immediately followed by a Full example link.
# 3. Finding all Full example links and ensuring each is immediately preceded by a ```cpp code block.
# 4. For each valid pair, it parses the link to get the source filename (e.g., .../my_example_8cpp-example.html -> my_example.cpp).
# 5. It normalizes both the snippet and the source file to be insensitive to indentation and line breaks.
# 6. It splits the normalized snippet into "chunks" using "..." as a delimiter and confirms that each chunk appears in the correct order within the normalized source file.
# All checks are performed at configure time, and any failure will result in a FATAL_ERROR.
file(READ ../README.md content)
set(remaining_content "${content}")
while (TRUE)
	# --- Find the next snippet block ---
	set(SNIPPET_START_MARKER "```cpp\n")
	string(FIND "${remaining_content}" "${SNIPPET_START_MARKER}" snippet_start_pos)
	if (snippet_start_pos EQUAL -1)
		# No more snippets found. Check for any orphaned links in the rest of the file.
		string(FIND "${remaining_content}" "[Full example](" orphaned_link_pos)
		if(NOT orphaned_link_pos EQUAL -1)
			message(FATAL_ERROR "README.md structure error: Found an orphaned '[Full example]' link without a preceding code snippet.")
		endif()
		break()
	endif()

	# --- Extract Snippet Content ---
	# "Eat" the content before and including the start marker.
	string(LENGTH "${SNIPPET_START_MARKER}" snippet_start_len)
	math(EXPR content_after_start_pos "${snippet_start_pos} + ${snippet_start_len}")
	string(SUBSTRING "${remaining_content}" ${content_after_start_pos} -1 content_after_start)

	set(SNIPPET_END_MARKER "\n```")
	string(FIND "${content_after_start}" "${SNIPPET_END_MARKER}" snippet_end_pos)
	if(snippet_end_pos EQUAL -1)
		message(FATAL_ERROR "README.md structure error: A ```cpp code block was found at position ${snippet_start_pos} but it is not properly closed with '```'.")
	endif()
	string(SUBSTRING "${content_after_start}" 0 ${snippet_end_pos} snippet_content)

	# --- Extract Link URL ---
	# "Eat" the snippet content and its end marker.
	string(LENGTH "${SNIPPET_END_MARKER}" snippet_end_len)
	math(EXPR content_after_snippet_pos "${snippet_end_pos} + ${snippet_end_len}")
	string(SUBSTRING "${content_after_start}" ${content_after_snippet_pos} -1 content_after_snippet)

	# Find the link, allowing for optional whitespace between snippet and link.
	string(REGEX MATCH "^[ \t\n\r]*\\[Full example\\]\\(([^\\)]+)\\)" match "${content_after_snippet}")
	if(NOT CMAKE_MATCH_COUNT EQUAL 1)
		message(FATAL_ERROR "README.md structure error: A ```cpp code block was found but it is NOT immediately followed by a '[Full example](...)' link as required. Please check the formatting.")
	endif()
	set(rtd_link "${CMAKE_MATCH_1}")

	# --- Advance remaining_content for the next loop iteration ---
	# Find the position of the link text we just matched.
	string(FIND "${content_after_snippet}" "${match}" link_match_pos)
	string(LENGTH "${match}" link_match_len)
	math(EXPR link_match_end_pos "${link_match_pos} + ${link_match_len}")

	# The next search should start after the link we just processed.
	math(EXPR next_search_start_pos_in_after_content "${content_after_snippet_pos} + ${link_match_end_pos}")
	string(SUBSTRING "${content_after_start}" ${next_search_start_pos_in_after_content} -1 remaining_content)

	# Extract filename from ReadTheDocs link
	string(REGEX MATCH "/([a-zA-Z0-9_]+)_8cpp-example\\.html$" _ "${rtd_link}")
	set(example_basename "${CMAKE_MATCH_1}")
	set(example_filename "${example_basename}.cpp")
	set(full_example_path "${CMAKE_CURRENT_SOURCE_DIR}/${example_filename}")

	if (NOT EXISTS "${full_example_path}")
		message(FATAL_ERROR "Example file '${full_example_path}' referenced by README.md snippet does not exist in tests/ directory.")
	endif()

	# --- Snippet Verification via Ordered Chunk Search ---
	# This method treats "..." as a wildcard delimiter. It splits the snippet into chunks
	# and verifies that each chunk appears in the full source file in the correct order.
	# To safely use ';' as a list separator for chunks, we first escape all literal
	# semicolons in both the snippet and the full source code.
	file(READ "${full_example_path}" full_content)

	# --- Whitespace Normalization Function ---
	# This function converts text to a canonical form for comparison by:
	# 1. Replacing all newlines with a single space.
	# 2. Collapsing all sequences of multiple horizontal spaces into a single space.
	# 3. Stripping any leading/trailing whitespace.
	# This makes the verification insensitive to line breaks and indentation.
	function(normalize_for_comparison input_text output_var)
		string(REPLACE "\n" " " temp_string "${input_text}") # Flatten to single line
		string(REGEX REPLACE "[ \t]+" " " temp_string "${temp_string}") # Collapse spaces
		string(STRIP "${temp_string}" normalized_output)
		set(${output_var} "${normalized_output}" PARENT_SCOPE)
	endfunction()

	string(REPLACE ";" "__SEMICOLON__" escaped_snippet "${snippet_content}")
	string(REPLACE ";" "__SEMICOLON__" escaped_full_content "${full_content}")

	# Normalize both the snippet and the full source into a canonical, single-line form.
	normalize_for_comparison("${escaped_snippet}" normalized_snippet)
	normalize_for_comparison("${escaped_full_content}" normalized_full_content)

	# Now that the snippet is a single line, we can simply replace "..." with a semicolon to create chunks.
	string(REPLACE "..." ";" snippet_chunks "${normalized_snippet}")

	set(remaining_source_content "${normalized_full_content}")

	foreach(chunk IN LISTS snippet_chunks)
		# If a chunk is empty (e.g., snippet starts/ends with "..." or has "... ..."), skip it.
		if ("${chunk}" STREQUAL "")
			continue()
		endif()

		# The chunks are already normalized, so we can search for them directly.
		string(FIND "${remaining_source_content}" "${chunk}" found_pos)
		if (found_pos EQUAL -1)
			message(FATAL_ERROR "README.md snippet verification failed!\nFile: ${example_filename}\nSnippet chunk not found (or not in correct order):\n'${chunk}'")
		endif()

		# "Eat" the content up to and including the found chunk to prepare for the next search.
		string(LENGTH "${chunk}" chunk_length)
		math(EXPR next_search_start_pos "${found_pos} + ${chunk_length}")
		string(SUBSTRING "${remaining_source_content}" ${next_search_start_pos} -1 remaining_source_content)
	endforeach()
	message(STATUS "Verified README snippet for ${example_filename}")

endwhile()

foreach(example IN ITEMS
		handling_errors_and_warnings
		file_type_determination
		path_to_text_stream
		local_embedding_similarity
		filter_emails_by_subject
		stream_to_html
		parse_archives
		local_ai_classify
		openai_classify
		local_ai_translate
		openai_translate
		local_ai_sentiment
		openai_sentiment
		local_ai_summary
		openai_voice_summary
		openai_transcribe_summary
		local_ai_find
		openai_find_image
		openai_embedding
		reuse_chain
		join_transformers
		xml_parsing_example)
	message(STATUS "Adding test ${example}")
	add_executable(${example} ${example}.cpp)
	target_include_directories(${example} PUBLIC ../src)
	target_link_libraries(${example} PRIVATE docwire_core docwire_office_formats docwire_mail
		docwire_ocr docwire_archives docwire_ai docwire_local_ai docwire_openai docwire_fuzzy_match
		docwire_content_type docwire_http)

	# Read the example file to check for OpenAI and local AI usage.
	file(READ ${example}.cpp example_content)
	string(FIND "${example_content}" "openai::" openai_pos)
	string(FIND "${example_content}" "OPENAI_API_KEY" openai_key_pos)

	# Conditionally add the test, skipping OpenAI examples if the key is not set.
	if ((openai_pos EQUAL -1 AND openai_key_pos EQUAL -1) OR NOT "$ENV{OPENAI_API_KEY}" STREQUAL "")
		add_test(NAME ${example} COMMAND ${example})
		if(WIN32)
			set_property(TEST ${example} APPEND PROPERTY ENVIRONMENT "${docwire_test_env_path}")
		endif()
		string(FIND "${example_content}" "local_ai::" local_ai_pos)
		if (NOT local_ai_pos EQUAL -1)
			message(STATUS "Labeling test ${example} as uses_model_runner")
			set_property(TEST ${example} PROPERTY LABELS "is_example;uses_model_runner")
		else()
			set_property(TEST ${example} PROPERTY LABELS "is_example")
		endif()
	else()
		message(STATUS "Skipping test for ${example} because it uses OpenAI and OPENAI_API_KEY is not set.")
	endif()
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
