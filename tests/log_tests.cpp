/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include <boost/config.hpp>
#include <boost/json.hpp>
#include "log.h"
#include "log_json_stream_sink.h"
#include "log_state_saver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <memory>
#include "serialization.h" // IWYU pragma: keep
#include <sstream>

using namespace docwire;

namespace
{
	std::string read_test_file(const std::string& file_name)
	{
		std::ifstream stream;
		stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		stream.open(file_name);
		return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
	}
}

std::string sanitize_log_text(const std::string& orig_log_text)
{
    using namespace boost::json;
	if (orig_log_text.empty()) {
		return "[\n]\n";
	}
	std::string log_text = "[\n";
	value log_val = parse(orig_log_text);
	for (int i = 0; i < log_val.as_array().size(); i++)
	{
		if (i > 0)
			log_text += ",\n";
		log_val.as_array()[i].as_object()["timestamp"] = "<timestamp>";
		log_val.as_array()[i].as_object()["thread_id"] = "<thread_id>";
		log_val.as_array()[i].as_object()["line"] = "<line>";
		log_text += serialize(log_val.as_array()[i]);
	}
	log_text += "\n]\n";
    return log_text;
}

std::string sanitize_expected_log_text(const std::string& orig_log_text)
{
    if constexpr (std::is_same_v<source_location, basic_source_location>)
    {
        // When using the fallback basic_source_location, __builtin_FUNCTION() may return a
        // bare function name (e.g., "TestBody") instead of the fully qualified one.
        // To make the test pass, we simplify the function name in the expected JSON
        // to match the actual output on these platforms.
        static const std::regex re{R"x("function":".*?(\w+)\s*\([^"]*\)")x"};
        return std::regex_replace(orig_log_text, re, R"y("function":"$1")y");
    }        
    else
    {
        return orig_log_text;
    }
}

TEST(Logging, Dereferenceable)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        log_entry(std::optional<int>(1));
        log_entry(std::optional<int>());
        log_entry(std::make_unique<int>(1));
        log_entry(std::unique_ptr<int>());
        log_entry(std::make_shared<int>(1));
        log_entry(std::shared_ptr<int>());
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
	ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_dereferenceable.out.json")), log_text);
#endif
}

TEST(Logging, Iterable)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        log_entry((std::vector<int>{1, 2, 3}));
        log_entry((std::list<std::string>{"a", "b", "c"}));
        std::vector<std::pair<std::string, int>> vec_of_pairs = {{"one", 1}, {"two", 2}};
        log_entry(vec_of_pairs);
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    // In release mode, none of these logs should be generated.
	ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_iterable.out.json")), log_text);
#endif
}

TEST(Logging, MemberVariable)
{
    struct Point { int x; int y; };
    struct Rect { Point top_left; Point bottom_right; };

    Rect r1 = {{1, 2}, {3, 4}};
    Rect r2 = {{5, 6}, {7, 8}};

	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        log_entry(r1.top_left.x, r2.top_left.x);
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_member_variable.out.json")), log_text);
#endif
}

TEST(Logging, CerrLogRedirection)
{
	std::stringstream log_stream;
    std::stringstream captured_cerr_stream;
    {
        log::state_saver saver;
	    log::set_sink(log::json_stream_sink(log_stream));
	    log::set_filter("*");
 
        // Redirect cerr to a stringstream to verify that nothing is written to it.
        std::streambuf* original_cerr_buf = std::cerr.rdbuf(captured_cerr_stream.rdbuf());

        {
            log::cerr_redirection cerr_redirection;
            std::cerr << "Cerr test log message line 1" << std::endl;
            std::cerr << "Cerr test log message line 2" << std::endl;
            // cerr_redirection's destructor will call restore()
        }

        // Restore original cerr
        std::cerr.rdbuf(original_cerr_buf);
    }

	std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
	// In release mode, the log entry is compiled out, and cerr output is suppressed.
	ASSERT_EQ("[\n]\n", log_text);
	ASSERT_TRUE(captured_cerr_stream.str().empty()) << "stderr should be empty in release mode as it's redirected to a null stream.";
#else
	ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_cerr_log_redirection.out.json")), log_text);
	ASSERT_TRUE(captured_cerr_stream.str().empty()) << "stderr should be empty in debug mode as it's redirected to the log.";
#endif
}

TEST(Logging, Basics)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        int my_var = 123;
        const char* my_cstr = "a c-string";
        log_entry("A literal string", my_var, my_cstr);
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    // In release mode, this log should be compiled out.
    ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_basics.out.json")), log_text);
#endif
}

TEST(Logging, Scope)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        int scope_var = 42;
        {
            log_scope(scope_var);
            log_entry("inside scope");
        }
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    // In release mode, the scope and entry logs should be compiled out.
    ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_scope.out.json")), log_text);
#endif
}

int function_with_log_return()
{
    return log_forward(123, log::return_value{});
}

TEST(Logging, LogForward)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        int a = 10;
        int b = 20;
        int c = log_forward(a + b);
        EXPECT_EQ(c, 30);

        int d = log_forward(c * 2, log::audit{});
        EXPECT_EQ(d, 60);

        EXPECT_EQ(function_with_log_return(), 123);
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_log_forward.release.out.json")), log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_log_forward.out.json")), log_text);
#endif
}

namespace
{
void function_for_log_test()
{
    log_entry(log::audit{}, "from function_for_log_test");
}
}

TEST(Logging, FilteringByFileAndFunction)
{
    std::stringstream log_stream;
    log::state_saver saver;
    log::set_sink(log::json_stream_sink(log_stream));

    // Test filename filtering
    // Also test combined file and function filters
    log::set_filter("@file:log_tests.cpp, @func:*TestBody*");
    log_entry(log::audit{}, "file_and_func_exact_match");
    ASSERT_THAT(log_stream.str(), testing::HasSubstr("file_and_func_exact_match"));
    log_stream.str("");

    log::set_filter("@file:*_tests.cpp");
    log_entry(log::audit{}, "file_suffix_wildcard");
    ASSERT_THAT(log_stream.str(), testing::HasSubstr("file_suffix_wildcard"));
    log_stream.str("");

    log::set_filter("@file:log_t?sts.cpp");
    log_entry(log::audit{}, "file_char_wildcard");
    ASSERT_THAT(log_stream.str(), testing::HasSubstr("file_char_wildcard"));
    log_stream.str("");

    log::set_filter("-@file:log_tests.cpp");
    log_entry(log::audit{}, "file_negative_filter");
    ASSERT_TRUE(log_stream.str().empty());
    log_stream.str("");

    // Test function name filtering
    // Note: The exact function name can vary between compilers. Wildcards are essential.
    log::set_filter("@func:*function_for_log_test*");
    function_for_log_test();
    ASSERT_THAT(log_stream.str(), testing::HasSubstr("from function_for_log_test"));
    log_stream.str("");

    log::set_filter("-@func:*function_for_log_test*");
    function_for_log_test();
    ASSERT_TRUE(log_stream.str().empty());
    log_stream.str("");

}

struct include_me { static constexpr std::string_view string() { return "include_me"; } };
struct special { static constexpr std::string_view string() { return "special"; } };

TEST(Logging, Filtering)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        
        // Filter to only include 'include_me' and 'scope_exit' tags, but exclude 'special'.
        log::set_filter("include_me,scope_exit,-special");
        log_entry("this is excluded");
        log_entry(log::audit{}, "this is also excluded by filter");
        log_entry(log::return_value{}, "excluded by tag");
        log_entry(include_me{}, "this is included");
        {
            log_scope(); // scope_enter is excluded by filter
            log_entry(include_me{}, "another included entry");
            log_entry(include_me{}, special{}, "this is excluded by negative filter");
        } // scope_exit is included
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    ASSERT_EQ("[\n]\n", log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_filtering.out.json")), log_text);
#endif
}

TEST(Logging, AuditInRelease)
{
	std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        log_entry("A regular debug log");
        log_entry(log::audit{}, "An important audit event");
    }

    std::string log_text = sanitize_log_text(log_stream.str());
#ifdef NDEBUG
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_audit.release.out.json")), log_text);
#else
    ASSERT_EQ(sanitize_expected_log_text(read_test_file("logging_audit.out.json")), log_text);
#endif
}
