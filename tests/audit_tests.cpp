#include "audit.hpp"
#include "audit/legacy_log_audit.hpp"
#include "log_core.h"
#include "log_json_stream_sink.h"
#include "log_state_saver.h"
#include "meta_data_exporter.h"
#include "named.h"
#include "source_location.h"
#include "xml_parser.h"

#include <gtest/gtest.h>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <memory>

using namespace docwire;

namespace
{

struct recording_auditor
{
    int record_count = 0;
    docwire::source_location last_location;

    template<typename... Args>
    void record(docwire::source_location location, Args&&...)
    {
        ++record_count;
        last_location = location;
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
    audit::entry(auditor, 42, "text");
    SUCCEED();
}

TEST(AuditEntry, RecordsUsingNonEmptyAuditor)
{
    recording_auditor auditor;
    audit::entry(auditor, 42, "text");

    EXPECT_EQ(auditor.record_count, 1);
    EXPECT_NE(std::string_view(auditor.last_location.function_name()).find("TestBody"),
              std::string_view::npos);
}

TEST(AuditScope, RecordsEnterAndExit)
{
    recording_auditor auditor;
    {
        audit::scope _{auditor, "scope_variable"_v = 123};
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
