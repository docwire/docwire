#include "json_serialization.h"
#include "serialization.h"
#include "gtest/gtest.h"
#include <cstdint>

using namespace docwire::serialization;

TEST(JsonSerialization, Null)
{
    value v = nullptr;
    EXPECT_EQ(to_json(v), "null");
}

TEST(JsonSerialization, True)
{
    value v = true;
    EXPECT_EQ(to_json(v), "true");
}

TEST(JsonSerialization, Int64)
{
    value v = std::int64_t{42};
    EXPECT_EQ(to_json(v), "42");
}

TEST(JsonSerialization, UInt64)
{
    value v = std::uint64_t{43};
    EXPECT_EQ(to_json(v), "43");
}

TEST(JsonSerialization, Double)
{
    value v = 3.14;
    EXPECT_EQ(to_json(v), "3.14E0");
}

TEST(JsonSerialization, String)
{
    value v = std::string{"hello"};
    EXPECT_EQ(to_json(v), "\"hello\"");
}

TEST(JsonSerialization, EmptyArray)
{
    array a;
    value v = a;
    EXPECT_EQ(to_json(v), "[]");
}

TEST(JsonSerialization, ArrayOfIntegers)
{
    array a;
    a.v.push_back(value{std::int64_t{1}});
    a.v.push_back(value{std::int64_t{2}});
    value v = a;
    EXPECT_EQ(to_json(v), "[1,2]");
}

TEST(JsonSerialization, EmptyObject)
{
    object o;
    value v = o;
    EXPECT_EQ(to_json(v), "{}");
}

TEST(JsonSerialization, Object)
{
    object o;
    o.v["key"] = value{std::int64_t{42}};
    value v = o;
    EXPECT_EQ(to_json(v), "{\"key\":42}");
}
