#include "audit.hpp"
#include "meta_data_exporter.h"
#include "named.h"
#include "source_location.h"
#include "stringification.h"
#include "xml_parser.h"

#include "gmock/gmock.h"
#include "audit/detail/filter_rules.hpp"
#include "audit/filter.hpp"
#include "audit/scope_tags.hpp"
#include "audit/serialize/json.hpp"
#include "audit/sink/ostream.hpp"
#include "named.h"

#include <gtest/gtest.h>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

using namespace docwire;

namespace
{

struct recording_auditor
{
    int record_count = 0;
    docwire::source_location last_location;
    std::vector<std::string> values;

    template<typename... Args>
    void record(docwire::source_location location, Args&&... args)
    {
        ++record_count;
        last_location = location;

        auto append_value = [this](auto&& result)
        {
            using result_type = std::decay_t<decltype(result)>;
            if constexpr (audit::detail::is_std_tuple_v<result_type>)
            {
                std::apply([this](auto&&... item) {
                    (values.push_back(stringify(std::forward<decltype(item)>(item))), ...);
                }, std::forward<decltype(result)>(result));
            }
            else
            {
                values.push_back(stringify(std::forward<decltype(result)>(result)));
            }
        };

        auto process_arg = [&](auto&& arg)
        {
            using arg_type = std::decay_t<decltype(arg)>;
            if constexpr (std::invocable<arg_type>)
            {
                append_value(std::invoke(std::forward<decltype(arg)>(arg)));
            }
            else
            {
                values.push_back(stringify(std::forward<decltype(arg)>(arg)));
            }
        };

        (process_arg(std::forward<Args>(args)), ...);
    }
};

struct non_empty_auditor
{
    int state = 0;
};

struct audit_test_tag
{
    static constexpr std::string_view string() { return "audit_test_tag"; }
};

struct excluded_tag
{
    static constexpr std::string_view string() { return "excluded_tag"; }
};

} // namespace

TEST(AuditEntry, NullAuditorDoesNothing)
{
    audit::null_auditor auditor;
    audit::entry(auditor, []{ return "text"; });
    SUCCEED();
}

TEST(AuditEntry, RecordsUsingNonEmptyAuditor)
{
    recording_auditor auditor;
    int value = 42;
    audit::entry(auditor, [&]{ return "value"_v = value; });

    EXPECT_EQ(auditor.record_count, 1);
    ASSERT_EQ(auditor.values.size(), 1);
    EXPECT_EQ(auditor.values[0], "value: 42");
    EXPECT_NE(std::string_view(auditor.last_location.function_name()).find("TestBody"),
              std::string_view::npos);
}

TEST(AuditEntry, StringLiteralContext)
{
    recording_auditor auditor;
    audit::entry(auditor, []{ return "plain string literal"; });

    EXPECT_EQ(auditor.record_count, 1);
    ASSERT_EQ(auditor.values.size(), 1);
    EXPECT_EQ(auditor.values[0], "plain string literal");
    EXPECT_NE(std::string_view(auditor.last_location.function_name()).find("TestBody"),
              std::string_view::npos);
}

TEST(AuditEntry, MultipleNamedValuesUsingCommaOperator)
{
    recording_auditor auditor;
    int a = 1;
    int b = 2;

    audit::entry(auditor, [&]{
        return "a"_v = a, "b"_v = b;
    });

    EXPECT_EQ(auditor.record_count, 1);
    ASSERT_EQ(auditor.values.size(), 2);
    EXPECT_EQ(auditor.values[0], "a: 1");
    EXPECT_EQ(auditor.values[1], "b: 2");
}

TEST(AuditEntry, WithPrecedingTag)
{
    recording_auditor auditor;
    bool evaluated = false;

    audit::entry(auditor, [&] {
        evaluated = true;
        return "value"_v = 7;
    }, audit_test_tag{});

    EXPECT_TRUE(evaluated);
    EXPECT_EQ(auditor.record_count, 1);
}

TEST(AuditScope, RecordsEnterAndExit)
{
    recording_auditor auditor;
    {
        audit::scope audit_scope{auditor, []{ return "scope_variable"_v = 123; }};
        EXPECT_EQ(auditor.record_count, 1);
        EXPECT_NE(std::string_view(auditor.last_location.function_name()).find("TestBody"),
                  std::string_view::npos);
    }
    EXPECT_EQ(auditor.record_count, 2);
}

TEST(AuditConstructor, DefaultConstructionConstraints)
{
    static_assert(std::is_default_constructible_v<xml_parser<strict, audit::null_auditor>>);
    static_assert(!std::is_default_constructible_v<xml_parser<strict, non_empty_auditor>>);

    static_assert(std::is_default_constructible_v<metadata_exporter<audit::null_auditor>>);
    static_assert(!std::is_default_constructible_v<metadata_exporter<non_empty_auditor>>);
}

TEST(AuditComposition, SerializesSharedPtr)
{
    std::stringstream log_stream;
    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};

        auditor.record(source_location::current(), []{ return std::make_shared<int>(42); });
    }

    std::string output = log_stream.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("42"), std::string::npos);
}

TEST(AuditComposition, FilterSpecParsing)
{
    auto wildcard_spec = audit::detail::parse_filter("*");
    EXPECT_TRUE(wildcard_spec.rules.empty());
    EXPECT_TRUE(wildcard_spec.wildcard_enabled);

    auto empty_spec = audit::detail::parse_filter("");
    EXPECT_TRUE(empty_spec.rules.empty());
    EXPECT_FALSE(empty_spec.wildcard_enabled);

    auto negative_spec = audit::detail::parse_filter("-@file:a.cpp");
    ASSERT_EQ(negative_spec.rules.size(), 1);
    EXPECT_TRUE(negative_spec.rules[0].is_negative);
    EXPECT_EQ(negative_spec.rules[0].type, audit::detail::filter_rule::FILE);
    EXPECT_EQ(negative_spec.rules[0].value, "a.cpp");

    auto tag_spec = audit::detail::parse_filter("audit_test_tag");
    ASSERT_EQ(tag_spec.rules.size(), 1);
    EXPECT_FALSE(tag_spec.rules[0].is_negative);
    EXPECT_EQ(tag_spec.rules[0].type, audit::detail::filter_rule::TAG);
    EXPECT_EQ(tag_spec.rules[0].value, "audit_test_tag");
}

TEST(AuditComposition, ComposedJsonStreamAuditor)
{
    std::stringstream log_stream;
    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};

        auditor.record(source_location::current(), "test message"_v = 42);
    }

    std::string output = log_stream.str();
    EXPECT_NE(output.find("test message"), std::string::npos);
    EXPECT_NE(output.find("42"), std::string::npos);
}

TEST(AuditComposition, FilteredComposedJsonStreamAuditor)
{
    std::stringstream included_stream;
    std::stringstream excluded_stream;
    {
        auto included_auditor =
            audit::filter::by_spec{"*"}
            | audit::serialize::json{}
            | audit::sink::ostream{included_stream};

        included_auditor.record(source_location::current(), "included message"_v = 42);
    }
    {
        auto excluded_auditor =
            audit::filter::by_spec{"nonexistent_tag"}
            | audit::serialize::json{}
            | audit::sink::ostream{excluded_stream};

        excluded_auditor.record(source_location::current(), "excluded message"_v = 99);
    }

    EXPECT_NE(included_stream.str().find("included message"), std::string::npos);
    EXPECT_EQ(excluded_stream.str().find("excluded message"), std::string::npos);
}

TEST(AuditComposition, EntryTagIsVisibleToFilter)
{
    std::stringstream matching_stream;
    std::stringstream non_matching_stream;

    {
        auto matching_auditor =
            audit::filter::by_spec{"audit_test_tag"}
            | audit::serialize::json{}
            | audit::sink::ostream{matching_stream};

        audit::entry(
            matching_auditor,
            [] { return "tagged value"_v = 7; },
            audit_test_tag{});
    }

    {
        auto non_matching_auditor =
            audit::filter::by_spec{"some_other_tag"}
            | audit::serialize::json{}
            | audit::sink::ostream{non_matching_stream};

        audit::entry(
            non_matching_auditor,
            [] { return "tagged value"_v = 8; },
            audit_test_tag{});
    }

    EXPECT_NE(matching_stream.str().find("tagged value"), std::string::npos);
    EXPECT_EQ(non_matching_stream.str().find("tagged value"), std::string::npos);
}

TEST(AuditSerialization, Dereferenceable)
{
    std::stringstream log_stream;
    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};

        auditor.record(source_location::current(), std::optional<int>(1));
        auditor.record(source_location::current(), std::optional<int>());
        auditor.record(source_location::current(), std::make_unique<int>(1));
        auditor.record(source_location::current(), std::unique_ptr<int>());
        auditor.record(source_location::current(), std::make_shared<int>(1));
        auditor.record(source_location::current(), std::shared_ptr<int>());
    }

    std::string output = log_stream.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("\"value\":1"), std::string::npos);
    EXPECT_NE(output.find("null"), std::string::npos);
}

TEST(AuditSerialization, Iterable)
{
    std::stringstream log_stream;
    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};

        auditor.record(source_location::current(), std::vector<int>{1, 2, 3});
        auditor.record(source_location::current(), std::list<std::string>{"a", "b", "c"});
        std::vector<std::pair<std::string, int>> vec_of_pairs = {{"one", 1}, {"two", 2}};
        auditor.record(source_location::current(), vec_of_pairs);
    }

    std::string output = log_stream.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("\"value\":\"a\""), std::string::npos);
    EXPECT_NE(output.find("\"value\":\"one\""), std::string::npos);
}

TEST(AuditSerialization, MemberVariable)
{
    struct point { int x; int y; };
    struct rect { point top_left; point bottom_right; };

    rect r1 = {{1001, 2}, {3, 4}};
    rect r2 = {{5005, 6}, {7, 8}};

    std::stringstream log_stream;
    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};

        auditor.record(source_location::current(), r1.top_left.x);
        auditor.record(source_location::current(), r2.top_left.x);
    }

    std::string output = log_stream.str();
    EXPECT_NE(output.find("1001"), std::string::npos);
    EXPECT_NE(output.find("5005"), std::string::npos);
}

TEST(AuditStderrRedirection, RedirectsCerr)
{
    std::stringstream log_stream;
    std::stringstream captured_cerr_stream;
    std::streambuf* original_cerr_buf = std::cerr.rdbuf(captured_cerr_stream.rdbuf());

    {
        auto auditor =
            audit::serialize::json{}
            | audit::sink::ostream{log_stream};
        using auditor_t = decltype(auditor);

        {
            audit::stderr_redirection<auditor_t> redir{audit::auditor_ref<auditor_t>{auditor}};
            std::cerr << "Cerr test log message" << std::endl;
        }
    }

    std::cerr.rdbuf(original_cerr_buf);

    EXPECT_TRUE(captured_cerr_stream.str().empty())
        << "stderr output should be redirected while stderr_redirection is active.";
}

TEST(AuditComposition, FileAndFunctionFilters)
{
    std::stringstream included_stream;
    {
        auto auditor =
            audit::filter::by_spec{"@file:audit_tests.cpp, @func:*TestBody*"}
            | audit::serialize::json{}
            | audit::sink::ostream{included_stream};

        audit::entry(auditor, [] { return "file_and_func_match"_v = 1; });
    }
    EXPECT_NE(included_stream.str().find("file_and_func_match"), std::string::npos);

    std::stringstream excluded_stream;
    {
        auto auditor =
            audit::filter::by_spec{"-@file:audit_tests.cpp"}
            | audit::serialize::json{}
            | audit::sink::ostream{excluded_stream};

        audit::entry(auditor, [] { return "should_be_excluded"_v = 2; });
    }
    EXPECT_EQ(excluded_stream.str().find("should_be_excluded"), std::string::npos);
}

TEST(AuditComposition, TagFiltering)
{
    std::stringstream included_stream;
    {
        auto auditor =
            audit::filter::by_spec{"audit_test_tag, -excluded_tag"}
            | audit::serialize::json{}
            | audit::sink::ostream{included_stream};

        audit::entry(auditor, [] { return "included message"_v = 1; }, audit_test_tag{});
        audit::entry(auditor, [] { return "excluded message"_v = 2; }, excluded_tag{});
    }
    EXPECT_NE(included_stream.str().find("included message"), std::string::npos);
    EXPECT_EQ(included_stream.str().find("excluded message"), std::string::npos);
}
