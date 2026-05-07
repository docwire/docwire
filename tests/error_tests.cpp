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

#include "diagnostic_message.h"
#include "error_hash.h" // IWYU pragma: keep
#include "error_tags.h"
#include "nested_exception.h"
#include "throw_if.h"
#include "source_location.h"
#include "log.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <boost/config.hpp>
#include <exception>
#include <filesystem>
#include <string>

using namespace docwire;

TEST (errors, throwing)
{
    ASSERT_NO_THROW(throw_if(2 > 3, "test"));
    try
    {
        throw make_error("test");
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(0), typeid(const char*));
        ASSERT_EQ(e.context_string(0), "test");
    }
    try
    {
        std::string s { "test" };
        throw make_error(s);
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(0), typeid(std::pair<std::string, std::string>));
        ASSERT_EQ(e.context_string(0), "s: test");
    }
    try
    {
        throw make_error(errors::network_failure{});
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(0), typeid(errors::network_failure));
        ASSERT_EQ(e.context_string(0), "network failure error tag");
    }
    try
    {
        std::string s { "test" };
        throw_if(2 < 3, errors::file_encrypted{}, s);
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_count(), 3);
        ASSERT_EQ(e.context_type(0), typeid(std::pair<std::string, const char *>));
        ASSERT_EQ(e.context_string(0), "triggering_condition: 2 < 3");
        ASSERT_EQ(e.context_type(1), typeid(errors::file_encrypted));
        ASSERT_EQ(e.context_string(1), "file encrypted error tag");
        ASSERT_EQ(e.context_type(2), typeid(std::pair<std::string, std::string>));
        ASSERT_EQ(e.context_string(2), "s: test");
    }
}

TEST (errors, diagnostic_message)
{
    std::string message;
    source_location err2_loc, err3_loc;
    try
    {
        try
        {
            try
            {
                throw std::runtime_error{"level 1 exception"};
            }
            catch (const std::exception& e)
            {
                std::string string_2{"string data 2"};
                err2_loc = source_location::current();
                std::throw_with_nested(make_error("level 2 exception", string_2));
            }
        }
        catch (const std::exception& e)
        {
            std::string string_3{"string data 3"};
            int int_3 = 3;
            err3_loc = source_location::current();
            std::throw_with_nested(make_error("level 3 exception", string_3, int_3, errors::program_logic{}));
        }
    }
    catch (const std::exception& e)
    {
        message = errors::diagnostic_message(e);
    }
	ASSERT_EQ(message,
        std::string{"Error: \"level 1 exception\"\n"} +
        "No location information available\n" +
        "wrapping at: " + err2_loc.function_name() + "\n" +
        "at " + err2_loc.file_name() + ":" + std::to_string(err2_loc.line() + 1) + "\n" +
		"with context \"level 2 exception\"\n" +
		"with context \"string_2: string data 2\"\n" +
        "wrapping at: " + err3_loc.function_name() + "\n" +
        "at " + err3_loc.file_name() + ":" + std::to_string(err3_loc.line() + 1) + "\n" +
		"with context \"level 3 exception\"\n" +
		"with context \"string_3: string data 3\"\n" +
		"with context \"int_3: 3\"\n" +
		"with context \"program logic error tag\"\n"
    );
}

template <typename Inner>
void test_make_nested(const Inner& inner)
{
    auto e = errors::make_nested(inner, std::logic_error{"level 2"}, std::runtime_error{"level 3"});
    static_assert(std::is_same_v<std::decay_t<decltype(e)>, errors::nested<std::runtime_error>>);
    ASSERT_STREQ(e.what(), "level 3");
    try
    {
        std::rethrow_if_nested(e);
        FAIL() << "Expected nested exception";
    }
    catch(const std::exception& e)
    {
        ASSERT_EQ(typeid(e), typeid(errors::nested<std::logic_error>));
        ASSERT_STREQ(e.what(), "level 2");
        try
        {
            std::rethrow_if_nested(e);
            FAIL() << "Expected nested exception";
        }
        catch(const std::exception& e)
        {
            ASSERT_EQ(typeid(e), typeid(std::out_of_range));
            ASSERT_STREQ(e.what(), "level 1");
        }
        catch(...)
        {
            FAIL() << "Unexpected exception";
        }
    }
    catch(...)
    {
        FAIL() << "Unexpected exception";
    }
}

TEST(errors, make_nested)
{
    test_make_nested(std::out_of_range{"level 1"});
}

TEST(errors, make_nested_current_exception)
{
    try
    {
        throw std::out_of_range{"level 1"};
    }
    catch (std::exception&)
    {
        test_make_nested(std::current_exception());
    }
    catch(...)
    {
        FAIL() << "Unexpected exception";
    }
}

TEST(errors, hashing)
{
    std::hash<errors::base> hasher;
    auto e1 = make_error("test");
    auto e2 = make_error("test");
    auto e3 = make_error("test");
    auto n1 = make_nested(e1, e3);
    auto n2 = make_nested(e2, e3);
    ASSERT_NE(hasher(e1), hasher(e2));
    ASSERT_NE(hasher(n1), hasher(n2));
    ASSERT_NE(hasher(n1), hasher(e1));
    ASSERT_NE(hasher(n2), hasher(e2));
    ASSERT_NE(hasher(n1), hasher(e3));
    ASSERT_NE(hasher(n2), hasher(e3));
    ASSERT_EQ(hasher(e1), hasher(e1));
    ASSERT_EQ(hasher(e2), hasher(e2));
    ASSERT_EQ(hasher(e3), hasher(e3));
    ASSERT_EQ(hasher(n1), hasher(n1));
    ASSERT_EQ(hasher(n2), hasher(n2));
}

namespace
{
    void check_caller_location(const docwire::basic_source_location& loc, int expected_line, const char* expected_func_substr)
    {
        // `file_name()` may return a full path, so we check the ending.
        EXPECT_TRUE(std::string(loc.file_name()).ends_with("error_tests.cpp") || std::string(loc.file_name()).ends_with("core_tests.cpp"));
        // `function_name()` may be decorated, so we check for a substring.
        EXPECT_THAT(std::string(loc.function_name()), testing::HasSubstr(expected_func_substr));
        // The line number must be the one passed from the call site.
        EXPECT_EQ(loc.line(), expected_line);
    }

    BOOST_NOINLINE void get_location_and_check()
    {
        check_caller_location(docwire::basic_source_location::current(), __LINE__, "get_location_and_check");
    }
}

TEST(SourceLocation, CustomImplementation)
{
    get_location_and_check();
}

TEST(DiagnosticContext, Concepts)
{
    static_assert(context_tag<log::audit>);
    static_assert(context_tag<log::stderr_redirect>);
    static_assert(context_tag<errors::program_logic>);
    static_assert(!context_tag<std::filesystem::path>);
    static_assert(!context_tag<int>);
    struct empty_struct {};
    static_assert(!context_tag<empty_struct>);
}
