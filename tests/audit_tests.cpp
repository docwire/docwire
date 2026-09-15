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
        (values.push_back(stringify(std::forward<Args>(args))), ...);
    }
};

struct non_empty_auditor
{
    int state = 0;
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
        legacy.record(source_location::current(), std::make_shared<int>(42));
    }

    std::string output = log_stream.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("42"), std::string::npos);
}
