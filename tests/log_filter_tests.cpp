#include "log_filter_rules.h"
#include "gtest/gtest.h"

using namespace docwire;

TEST(LogFilter, ParseWildcard)
{
    auto spec = log::detail::parse_log_filter("*");
    EXPECT_TRUE(spec.rules.empty());
    EXPECT_TRUE(spec.wildcard_enabled);
}

TEST(LogFilter, ParseEmpty)
{
    auto spec = log::detail::parse_log_filter("");
    EXPECT_TRUE(spec.rules.empty());
    EXPECT_FALSE(spec.wildcard_enabled);
}

TEST(LogFilter, ParseNegativeRule)
{
    auto spec = log::detail::parse_log_filter("-@file:a.cpp");
    ASSERT_EQ(spec.rules.size(), 1);
    EXPECT_TRUE(spec.rules[0].is_negative);
    EXPECT_EQ(spec.rules[0].type, log::detail::filter_rule::FILE);
    EXPECT_EQ(spec.rules[0].value, "a.cpp");
}
