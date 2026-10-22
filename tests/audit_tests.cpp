#include "audit.hpp"
#include "audit/legacy_log_audit.hpp"
#include "log_core.h"
#include "log_json_stream_sink.h"
#include "log_state_saver.h"
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
#include <memory>
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
    static_assert(std::is_default_constructible_v<xml_parser<strict, audit::legacy_log_audit>>);
    static_assert(!std::is_default_constructible_v<xml_parser<strict, non_empty_auditor>>);

    static_assert(std::is_default_constructible_v<metadata_exporter<audit::null_auditor>>);
    static_assert(std::is_default_constructible_v<metadata_exporter<audit::legacy_log_audit>>);
    static_assert(!std::is_default_constructible_v<metadata_exporter<non_empty_auditor>>);
}

TEST(LegacyLogAudit, SerializesSharedPtr)
{
    std::stringstream log_stream;
    {
        log::state_saver saver;
        log::set_sink(log::json_stream_sink(log_stream));
        log::set_filter("*");

        audit::legacy_log_audit legacy;
        legacy.record(source_location::current(), []{ return std::make_shared<int>(42); });
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
