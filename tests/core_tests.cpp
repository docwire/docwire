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

#include "chaining.h"
#include "convert_chrono.h" // IWYU pragma: keep
#include "ensure.h"
#include "lru_memory_cache.h"
#include "named.h"
#include "not_null.h"
#include "unique_identifier.h"
#include "tuple_utils.h"
#include "ref_or_owned.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <memory>
#include <functional>
#include <chrono>

using namespace docwire;

TEST(unique_identifier, generation_uniqueness_copying_and_hashing)
{
    std::vector<unique_identifier> identifiers(10);
    std::vector<unique_identifier> identifiers_copy{ identifiers };
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
        {
            if (i == j)
            {
                ASSERT_EQ(identifiers[i], identifiers_copy[j]);
                ASSERT_EQ(std::hash<unique_identifier>()(identifiers[i]), std::hash<unique_identifier>()(identifiers[j]));
            }
            else
            {
                ASSERT_NE(identifiers[i], identifiers_copy[j]);
                ASSERT_NE(identifiers[i], identifiers[j]);
            }
        }
}

TEST(lru_cache, storing_and_retrieving_values)
{
    lru_memory_cache<std::string, std::string> cache;
    for (int i = 0; i < 10; i++)
        cache.get_or_create("key" + std::to_string(i), [](const std::string& key) { return key + " cached value"; });
    for (int i = 0; i < 10; i++)
        ASSERT_EQ(cache.get_or_create("key" + std::to_string(i), [](const std::string& key) { return key + " new value"; }), "key" + std::to_string(i) + " cached value");
}

TEST(Convert, Chrono)
{
    using namespace docwire::serialization;

    // Test std::chrono::sys_seconds
    using namespace std::chrono;
    auto tp = sys_days{year{2024}/6/15} + hours{10} + minutes{30};
    std::string str_time = convert::to<std::string>(std::chrono::sys_seconds{tp});
    EXPECT_EQ(str_time, "2024-06-15 10:30:00");
}

TEST(Named, StructuredBinding)
{
    auto v = "test_name"_v = 123;
    auto [name, value] = v;
    ASSERT_EQ(name, "test_name");
    ASSERT_EQ(value, 123);

    static_assert(std::is_same_v<decltype(name), std::string_view>);
    static_assert(std::is_same_v<decltype(value), int>);
}

TEST(Named, StructuredBindingRef)
{
    auto v = "test_name"_v = 123;
    auto& [name, value] = v;
    ASSERT_EQ(name, "test_name");
    ASSERT_EQ(value, 123);

    value = 456;
    ASSERT_EQ(v.value, 456);
}

TEST(Named, StructuredBindingMove)
{
    auto v = "test_name"_v = std::string("test_value");
    auto [name, value] = std::move(v);
    ASSERT_EQ(name, "test_name");
    ASSERT_EQ(value, "test_value");
}

TEST(NotNull, ConstructionAndAccess)
{
    int value = 42;
    not_null<int*> ptr(&value);
    EXPECT_EQ(*ptr, 42);
    *ptr = 43;
    EXPECT_EQ(value, 43);

    struct S { int x; };
    S s{10};
    not_null<S*> s_ptr(&s);
    EXPECT_EQ(s_ptr->x, 10);
    s_ptr->x = 11;
    EXPECT_EQ(s.x, 11);
}

TEST(NotNull, SmartPointers)
{
    auto sp = std::make_shared<int>(100);
    not_null<std::shared_ptr<int>> nn_sp(sp);
    EXPECT_EQ(*nn_sp, 100);
    EXPECT_EQ(nn_sp.get(), sp.get());

    auto up = std::make_unique<int>(200);
    not_null<std::unique_ptr<int>> nn_up(std::move(up));
    EXPECT_EQ(*nn_up, 200);
}

TEST(NotNull, ThrowsOnNull)
{
    int* null_ptr = nullptr;
    EXPECT_THROW((not_null<int*, strict>(null_ptr)), docwire::errors::base);

    std::shared_ptr<int> null_sp;
    EXPECT_THROW((not_null<std::shared_ptr<int>, strict>(null_sp)), docwire::errors::base);
}

TEST(NotNull, AssumeNotNull)
{
    int value = 5;
    auto nn = assume_not_null(&value);
    EXPECT_EQ(*nn, 5);
}

TEST(Ensure, CorrectUsage)
{
    // Test successful comparisons
    ASSERT_NO_THROW(ensure(5) == 5);
    ASSERT_NO_THROW(ensure(5) != 6);
    ASSERT_NO_THROW(ensure(6) > 5);
    ASSERT_NO_THROW(ensure(5) < 6);
    ASSERT_NO_THROW(ensure(5) >= 5);
    ASSERT_NO_THROW(ensure(6) >= 5);
    ASSERT_NO_THROW(ensure(5) <= 5);
    ASSERT_NO_THROW(ensure(5) <= 6);
    ASSERT_NO_THROW(ensure(std::string("hello world")).contains("world"));
}

TEST(Ensure, ThrowsOnFailure)
{
    // Test failing comparisons
    ASSERT_THROW(ensure(5) == 6, docwire::errors::base);
    ASSERT_THROW(ensure(5) != 5, docwire::errors::base);
    ASSERT_THROW(ensure(5) > 6, docwire::errors::base);
    ASSERT_THROW(ensure(6) < 5, docwire::errors::base);
    ASSERT_THROW(ensure(5) >= 6, docwire::errors::base);
    ASSERT_THROW(ensure(6) <= 5, docwire::errors::base);
    ASSERT_THROW(ensure(std::string("hello world")).contains("galaxy"), docwire::errors::base);
}

TEST(Ensure, ThrowsWithCorrectContext)
{
    try
    {
        int actual = 42;
        int expected = 100;
        ensure(actual) == expected;
        FAIL() << "Expected ensure to throw";
    }
    catch (const errors::base& e)
    {
        std::string msg = docwire::errors::diagnostic_message(e);
        EXPECT_THAT(msg, testing::HasSubstr("!(m_value == other)"));
        EXPECT_THAT(msg, testing::HasSubstr("m_value: 42"));
        EXPECT_THAT(msg, testing::HasSubstr("other: 100"));
    }
}

#ifndef NDEBUG
TEST(EnsureDeathTest, MisuseDetection)
{
    // This test checks that using ensure() without a comparison operator
    // triggers an assertion failure in debug builds.
    ASSERT_DEATH(
        (void)ensure(2 == 3), // Incorrect usage
        "ensure\\(\\) was called without a comparison operator");
}
#endif

TEST(tuple_utils, subrange)
{
    static_assert(std::is_same_v<
        tuple_utils::subrange_t<1, 3, std::tuple<int, float, double, std::string, char>>,
        std::tuple<float, double, std::string>
    >);
    ASSERT_EQ(
        (tuple_utils::subrange<1, 3>(std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"}, char{4}))),
        std::make_tuple(float{1}, double{2}, std::string{"3"}));
}

TEST(tuple_utils, remove_first)
{
    static_assert(std::is_same_v<
        tuple_utils::remove_first_t<std::tuple<int, float, double, std::string, char>>,
        std::tuple<float, double, std::string, char>
    >);
    ASSERT_EQ(
        tuple_utils::remove_first(std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"}, char{4})),
        std::make_tuple(float{1}, double{2}, std::string{"3"}, char{4})
    );
}

TEST(tuple_utils, remove_last)
{
    static_assert(std::is_same_v<
        tuple_utils::remove_last_t<std::tuple<int, float, double, std::string, char>>,
        std::tuple<int, float, double, std::string>
    >);
    ASSERT_EQ(
        tuple_utils::remove_last(std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"}, char{4})),
        std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"})
    );
}

TEST(tuple_utils, first_element)
{
    static_assert(std::is_same_v<
        tuple_utils::first_element_t<std::tuple<int, float, double, std::string, char>>,
        int
    >);
    ASSERT_EQ(
        tuple_utils::first_element(std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"}, char{4})),
        int{0}
    );
}

TEST(tuple_utils, last_element)
{
    static_assert(std::is_same_v<
        tuple_utils::last_element_t<std::tuple<int, float, double, std::string, char>>,
        char
    >);
    ASSERT_EQ(
        tuple_utils::last_element(std::make_tuple(int{0}, float{1}, double{2}, std::string{"3"}, char{4})),
        char{4}
    );
}

TEST(chaining, val_temp_to_func_ref_one_arg_no_result)
{
    using namespace chaining;
    int result = 0;
    auto f = [&result](int value)->void { result = value + 2; };
    int{1} | f;
    ASSERT_EQ(result, 3);
}

TEST(chaining, val_ref_to_func_temp_two_args_with_result)
{
    using namespace chaining;
    int v = 2;
    auto binding = v | [](int value1, int value2)->int { return value1 + value2; };
    ASSERT_EQ(binding(1), 3);
}

TEST(chaining, func_temp_no_args_no_result_callback_no_result_to_func_ref_one_arg_no_result)
{
    using namespace chaining;
    int result = 0;
    auto f = [&result](int value) { result = value + 2; };
    [](std::function<void(int)> callback) { callback(1); } | f;
    ASSERT_EQ(result, 3);
}

TEST(chaining, func_temp_no_args_with_result_callback_with_result_to_func_temp_one_arg_with_result)
{
    using namespace chaining;
    auto binding =
        [](int value, std::function<int(int)> callback) { return callback(value); } |
        [](int value) { return value + 2; };
    ASSERT_EQ(binding(1), 3);
}

TEST(chaining, func_temp_no_args_no_result_callback_no_result_to_pushable_ref)
{
    using namespace chaining;
    std::vector<int> container;
    [](std::function<void(int)> callback) { callback(1); } | container;
    ASSERT_THAT(container, testing::ElementsAre(1));
}

TEST(chaining, func_ref_one_arg_with_result_callback_with_result_to_pushable_ref)
{
    using namespace chaining;
    std::vector<int> container;
    auto binding = [](int value, std::function<std::optional<int>(int)> callback) { return callback(value + 2); } | container;
    std::optional<int> result = binding(1);
    ASSERT_EQ(result, std::optional<int>{});
    ASSERT_THAT(container, testing::ElementsAre(3));
}

TEST(chaining, val_const_temp_to_func_one_arg_no_result_callback_no_result_to_pushable_ref)
{
    using namespace chaining;
    std::vector<int> container;
    1 | [](int value, std::function<void(int)> callback) { callback(value + 2); } | container;
    ASSERT_THAT(container, testing::ElementsAre(3));
}

TEST(chaining, func_temp_no_args_no_result_callback_no_result_to_func_one_arg_no_result_callback_no_result_to_pushable_ref)
{
    using namespace chaining;
    std::vector<int> container;
    [](std::function<void(int)> callback) { callback(1); } |
        [](int value, std::function<void(int)> callback) { callback(value + 2); } |
        container;
    ASSERT_THAT(container, testing::ElementsAre(3));
}

struct NonCopyableFunctor
{
    NonCopyableFunctor() = default;
    NonCopyableFunctor(const NonCopyableFunctor&) = delete;
    NonCopyableFunctor(NonCopyableFunctor&&) = default;
    int operator()(int value) const { return value + 2; }
};

TEST(chaining, func_temp_no_args_with_result_callback_with_result_to_non_copyable_functor_temp)
{
    using namespace chaining;
    int result = [](std::function<int(int)> callback) { return callback(1); } | NonCopyableFunctor{};
    ASSERT_EQ(result, 3);
}

TEST(chaining, func_temp_no_args_with_result_callback_with_result_to_non_copyable_functor_ref)
{
    using namespace chaining;
    NonCopyableFunctor ncf{};
    int result = [](const std::function<int(int)>& callback) { return callback(1); } | ncf;
    ASSERT_EQ(result, 3);
}

template<typename RefOrOwnedType, typename ValueType>
void test_ref_or_owned(int expected_result)
{
    ValueType v;

    ref_or_owned<RefOrOwnedType> ref{v};
    ASSERT_EQ(ref.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> ref_copied{ref};
    ASSERT_EQ(ref_copied.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> ref_moved{std::move(ref)};
    ASSERT_EQ(ref_moved.get().value(), expected_result);

    ref_or_owned<RefOrOwnedType> owned{ValueType{}};
    ASSERT_EQ(owned.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> owned_copied{owned};
    ASSERT_EQ(owned_copied.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> owned_moved{std::move(owned)};
    ASSERT_EQ(owned_moved.get().value(), expected_result);

    ref_or_owned<RefOrOwnedType> shared{std::make_shared<ValueType>()};
    ASSERT_EQ(shared.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> shared_copied{shared};
    ASSERT_EQ(shared_copied.get().value(), expected_result);
    ref_or_owned<RefOrOwnedType> shared_moved{std::move(shared)};
    ASSERT_EQ(shared_moved.get().value(), expected_result);
}

TEST(ref_or_owned, general)
{
    struct TestBase
    {
        TestBase() : m_value(1) {};
        TestBase(TestBase&)
        {
            throw std::runtime_error{"copy constructor called"};
        }
        TestBase(TestBase&&) = default;
        virtual ~TestBase() = default;
        int value() { return m_value; }
        int m_value;
    };
    struct TestDerived : TestBase
    {
        TestDerived() { m_value = 2; };
    };
    test_ref_or_owned<TestBase, TestBase>(1);
    test_ref_or_owned<TestBase, TestDerived>(2);
}
