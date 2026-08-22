#include "static_flat_map.h"
#include "gtest/gtest.h"
#include <array>
#include <functional>
#include <string>
#include <typeindex>

using namespace docwire;

TEST(StaticFlatMap, SortsInput)
{
    using map_type = static_flat_map<int, std::string, 3>;
    std::array<map_type::value_type, 3> items{{
        {3, "three"}, {1, "one"}, {2, "two"}
    }};
    map_type m{items};
    auto sorted = m.items();
    ASSERT_EQ(sorted.size(), 3);
    EXPECT_EQ(sorted[0].key, 1);
    EXPECT_EQ(sorted[1].key, 2);
    EXPECT_EQ(sorted[2].key, 3);
}

TEST(StaticFlatMap, FindExistingKey)
{
    using map_type = static_flat_map<int, std::string, 2>;
    std::array<map_type::value_type, 2> items{{
        {1, "one"}, {2, "two"}
    }};
    map_type m{items};
    EXPECT_EQ(*m.find(1), "one");
    EXPECT_EQ(*m.find(2), "two");
}

TEST(StaticFlatMap, FindMissingReturnsNullptr)
{
    using map_type = static_flat_map<int, std::string, 1>;
    std::array<map_type::value_type, 1> items{{
        {1, "one"}
    }};
    map_type m{items};
    EXPECT_EQ(m.find(42), nullptr);
}

TEST(StaticFlatMap, ItemsReturnsSorted)
{
    using map_type = static_flat_map<std::string, int, 3>;
    std::array<map_type::value_type, 3> items{{
        {"b", 2}, {"a", 1}, {"c", 3}
    }};
    map_type m{items};
    auto sorted = m.items();
    ASSERT_EQ(sorted.size(), 3);
    EXPECT_EQ(sorted[0].key, "a");
    EXPECT_EQ(sorted[1].key, "b");
    EXPECT_EQ(sorted[2].key, "c");
}

TEST(StaticFlatMap, TypeIndexKeys)
{
    using map_type = static_flat_map<std::type_index, std::string, 2>;
    std::array<map_type::value_type, 2> items{{
        {typeid(int), "int"}, {typeid(double), "double"}
    }};
    map_type m{items};
    EXPECT_EQ(*m.find(typeid(int)), "int");
    EXPECT_EQ(*m.find(typeid(double)), "double");
}

TEST(StaticFlatMap, CustomComparator)
{
    using map_type = static_flat_map<int, std::string, 2, std::greater<int>>;
    std::array<map_type::value_type, 2> items{{
        {1, "one"}, {2, "two"}
    }};
    map_type m{items};
    auto sorted = m.items();
    ASSERT_EQ(sorted.size(), 2);
    EXPECT_EQ(sorted[0].key, 2);
    EXPECT_EQ(sorted[1].key, 1);
}
