/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#include "concepts.h"
#include "data_source.h"
#include "serialization.h"
#include "stringification.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <filesystem>
#include <variant>

using namespace docwire;

TEST(Concepts, Basics)
{
    // container: Should be true for standard containers, false for others.
    static_assert(container<std::vector<int>>);
    static_assert(container<std::list<std::string>>);
    static_assert(container<std::map<int, double>>); // map is a container of pairs
    static_assert(container<std::set<char>>);
    static_assert(container<std::string>); // A string is a container of chars, but should be serialized as a value.
    static_assert(container<char[10]>); // A C-style array is a container of chars

    static_assert(!container<int>);
    static_assert(!container<const char*>); // A pointer is not a container
    static_assert(!container<std::optional<int>>);
    static_assert(!container<std::unique_ptr<int>>);
    static_assert(!container<std::filesystem::path>); // Correctly avoids self-recursion

    // dereferenceable: Should be true for pointer-like types.
    static_assert(dereferenceable<int*>);
    static_assert(dereferenceable<const int*>);
    static_assert(dereferenceable<std::unique_ptr<int>>);
    static_assert(dereferenceable<std::shared_ptr<std::string>>);
    static_assert(dereferenceable<std::optional<double>>);

    static_assert(!dereferenceable<int>);
    static_assert(!dereferenceable<std::string>);
    static_assert(!dereferenceable<std::vector<int>>); // It's a container, not pointer-like
    static_assert(!dereferenceable<std::filesystem::path>);

    // string_like: Should be true for string types and literals.
    static_assert(string_like<std::string>);
    static_assert(string_like<const std::string>);
    static_assert(string_like<std::string_view>);
    static_assert(string_like<const char*>);
    static_assert(string_like<char*>);
    static_assert(string_like<const char[10]>);
    static_assert(string_like<char[10]>);

    static_assert(!string_like<int>);
    static_assert(!string_like<std::vector<char>>);
}

TEST(Serialization, SpecializationLogic)
{
    using namespace docwire::serialization;

    // The combination of concepts should route types to the correct serializer.
    // We verify this by checking the `kind` of the resolved serializer specialization.

    // Primitives should use the Arithmetic or ValueAlternative serializers.
    static_assert(serializer<int>::kind == serializer_kind::arithmetic);
    static_assert(serializer<bool>::kind == serializer_kind::value_alternative);

    // std::string is a direct value alternative. Other string-like types use the string_like serializer.
    static_assert(serializer<std::string>::kind == serializer_kind::value_alternative);
    static_assert(serializer<const char*>::kind == serializer_kind::string_like);

    // Containers should use the Container serializer.
    static_assert(serializer<std::vector<int>>::kind == serializer_kind::container);

    // Pointer-like types should use the Dereferenceable serializer.
    static_assert(serializer<std::optional<int>>::kind == serializer_kind::dereferenceable);
    static_assert(serializer<std::unique_ptr<int>>::kind == serializer_kind::dereferenceable);
}

TEST(Serialization, PureSerialization)
{
    using namespace docwire::serialization;

    // Test primitive types
    value v_int = full(42);
    ASSERT_TRUE(std::holds_alternative<std::int64_t>(v_int));
    EXPECT_EQ(std::get<std::int64_t>(v_int), 42);

    value v_str = full(std::string("hello"));
    ASSERT_TRUE(std::holds_alternative<std::string>(v_str));
    EXPECT_EQ(std::get<std::string>(v_str), "hello");

    // Test a complex type (data_source)
    docwire::data_source ds(std::filesystem::path("test.txt"));
    value v_ds = full(ds);
    ASSERT_TRUE(std::holds_alternative<object>(v_ds));
    const auto& obj_ds = std::get<object>(v_ds);
    // With std::map, fields are sorted alphabetically
    ASSERT_EQ(obj_ds.v.size(), 2);
    EXPECT_EQ(std::get<std::string>(std::get<object>(std::get<object>(obj_ds.v.at("file_extension")).v.at("value")).v.at("value")), ".txt");
    EXPECT_EQ(std::get<std::string>(std::get<object>(obj_ds.v.at("path")).v.at("value")), "test.txt");
    // Ensure no typeid field is present in pure serialization
    EXPECT_EQ(obj_ds.v.count("typeid"), 0);
}

TEST(Serialization, PrimitivesAndEnums)
{
    using namespace docwire::serialization;

    // Test bool
    value v_bool = full(true);
    ASSERT_TRUE(std::holds_alternative<bool>(v_bool));
    EXPECT_TRUE(std::get<bool>(v_bool));

    // Test double
    value v_double = full(3.14);
    ASSERT_TRUE(std::holds_alternative<double>(v_double));
    EXPECT_DOUBLE_EQ(std::get<double>(v_double), 3.14);

    // Test enum
    value v_enum = full(docwire::confidence::very_high);
    ASSERT_TRUE(std::holds_alternative<std::string>(v_enum));
    EXPECT_EQ(std::get<std::string>(v_enum), "very_high");
}

TEST(Serialization, Containers)
{
    using namespace docwire::serialization;

    // Test std::vector
    std::vector<int> vec = {1, 2, 3};
    value v_vec = full(vec);
    ASSERT_TRUE(std::holds_alternative<array>(v_vec));
    const auto& arr = std::get<array>(v_vec);
    ASSERT_EQ(arr.v.size(), 3);
    EXPECT_EQ(std::get<std::int64_t>(arr.v[0]), 1);
    EXPECT_EQ(std::get<std::int64_t>(arr.v[1]), 2);
    EXPECT_EQ(std::get<std::int64_t>(arr.v[2]), 3);

    // Test std::pair
    auto p = std::make_pair(std::string("key"), 42);
    value v_pair = full(p);
    ASSERT_TRUE(std::holds_alternative<object>(v_pair));
    const auto& obj_pair = std::get<object>(v_pair);
    ASSERT_EQ(obj_pair.v.size(), 2);
    EXPECT_EQ(std::get<std::string>(obj_pair.v.at("first")), "key");
    EXPECT_EQ(std::get<std::int64_t>(obj_pair.v.at("second")), 42);
}

TEST(Serialization, PointersAndOptionals)
{
    using namespace docwire::serialization;

    // Test std::optional
    value v_opt_full = full(std::optional<int>(123));
    ASSERT_TRUE(std::holds_alternative<object>(v_opt_full));
    EXPECT_EQ(std::get<std::int64_t>(std::get<object>(v_opt_full).v.at("value")), 123);

    value v_opt_empty = full(std::optional<int>());
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(v_opt_empty));

    // Test std::unique_ptr
    value v_uniq_full = full(std::make_unique<int>(456));
    ASSERT_TRUE(std::holds_alternative<object>(v_uniq_full));
    EXPECT_EQ(std::get<std::int64_t>(std::get<object>(v_uniq_full).v.at("value")), 456);

    std::unique_ptr<int> empty_uniq;
    value v_uniq_empty = full(empty_uniq);
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(v_uniq_empty));
}

TEST(Serialization, StringLike)
{
    using namespace docwire::serialization;

    const char* null_str = nullptr;
    value result_null = full(null_str);
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(result_null));

    const char* non_null_str = "hello";
    value result_non_null = full(non_null_str);
    ASSERT_TRUE(std::holds_alternative<std::string>(result_non_null));
    EXPECT_EQ(std::get<std::string>(result_non_null), "hello");

    value result_sv = full(std::string_view{"world"});
    ASSERT_TRUE(std::holds_alternative<std::string>(result_sv));
    EXPECT_EQ(std::get<std::string>(result_sv), "world");
}

TEST(Serialization, StdTypes)
{
    using namespace docwire::serialization;

    // Test std::filesystem::path
    value v_path = full(std::filesystem::path("/tmp/test.file"));
    ASSERT_TRUE(std::holds_alternative<std::string>(v_path));
    EXPECT_EQ(std::get<std::string>(v_path), "/tmp/test.file");

    // Test std::exception
    value v_exc = full(std::runtime_error("An error occurred"));
    ASSERT_TRUE(std::holds_alternative<object>(v_exc));
    EXPECT_EQ(std::get<std::string>(std::get<object>(v_exc).v.at("what")), "An error occurred");
}

TEST(Serialization, TypedSummaryPrimitives)
{
    using namespace docwire::serialization;

    // Test int
    value v_int = typed_summary(42);
    ASSERT_TRUE(std::holds_alternative<object>(v_int));
    const auto& obj_int = std::get<object>(v_int);
    EXPECT_EQ(std::get<std::string>(obj_int.v.at("typeid")), "int");
    EXPECT_EQ(std::get<std::int64_t>(obj_int.v.at("value")), 42);

    // Test bool
    value v_bool = typed_summary(false);
    ASSERT_TRUE(std::holds_alternative<object>(v_bool));
    const auto& obj_bool = std::get<object>(v_bool);
    EXPECT_EQ(std::get<std::string>(obj_bool.v.at("typeid")), "bool");
    EXPECT_EQ(std::get<bool>(obj_bool.v.at("value")), false);

    // Test string literal
    value v_literal = typed_summary("literal");
    ASSERT_TRUE(std::holds_alternative<object>(v_literal));
    const auto& obj_literal = std::get<object>(v_literal);
    EXPECT_EQ(std::get<std::string>(obj_literal.v.at("typeid")), "char[8]");
    EXPECT_EQ(std::get<std::string>(obj_literal.v.at("value")), "literal");
}

TEST(Serialization, TypedSummaryString)
{
    using namespace docwire::serialization;
    value v_str = typed_summary(std::string("hello"));
    ASSERT_TRUE(std::holds_alternative<object>(v_str));
    const auto& obj_str = std::get<object>(v_str);
    EXPECT_EQ(std::get<std::string>(obj_str.v.at("typeid")), "std::string");
    EXPECT_EQ(std::get<std::string>(obj_str.v.at("value")), "hello");
}

TEST(Serialization, TypedSummaryComplex)
{
    using namespace docwire::serialization;

    // Test a complex type (data_source)
    docwire::data_source ds(std::filesystem::path("test.txt"));
    value v_ds = typed_summary(ds);
    ASSERT_TRUE(std::holds_alternative<object>(v_ds));
    const auto& obj_ds = std::get<object>(v_ds);
    ASSERT_EQ(obj_ds.v.size(), 2); // typeid and value
    EXPECT_EQ(std::get<std::string>(obj_ds.v.at("typeid")), "docwire::data_source");
    const auto& inner_val = std::get<object>(obj_ds.v.at("value")); // This is the object containing "path" and "file_extension"

    // Check path within data_source
    const auto& path_typed_summary = std::get<object>(inner_val.v.at("path")); // typed_summary(data.path())
    EXPECT_EQ(std::get<std::string>(path_typed_summary.v.at("typeid")), "std::optional<std::filesystem::path>");
    const auto& path_value_obj = std::get<object>(path_typed_summary.v.at("value")); // typed_summary(*data.path())
    EXPECT_EQ(std::get<std::string>(path_value_obj.v.at("typeid")), "std::filesystem::path");
    EXPECT_EQ(std::get<std::string>(path_value_obj.v.at("value")), "test.txt");

    // Check file_extension within data_source
    const auto& fe_typed_summary = std::get<object>(inner_val.v.at("file_extension")); // typed_summary(data.file_extension())
    EXPECT_EQ(std::get<std::string>(fe_typed_summary.v.at("typeid")), "std::optional<docwire::file_extension>");
    const auto& fe_value_obj = std::get<object>(fe_typed_summary.v.at("value")); // typed_summary(*data.file_extension())
    EXPECT_EQ(std::get<std::string>(fe_value_obj.v.at("typeid")), "docwire::file_extension");
    const auto& fe_inner_value_obj = std::get<object>(fe_value_obj.v.at("value")); // full(ext)
    EXPECT_EQ(std::get<std::string>(fe_inner_value_obj.v.at("value")), ".txt");

    // Test nested typed serialization
    auto pair = std::make_pair(std::string("key"), ds);
    value v_pair = typed_summary(pair);
    ASSERT_TRUE(std::holds_alternative<object>(v_pair));
    const auto& obj_pair = std::get<object>(v_pair);
    ASSERT_EQ(obj_pair.v.size(), 2); // "typeid" and "value"
    EXPECT_EQ(std::get<std::string>(obj_pair.v.at("typeid")), "std::pair<std::string,docwire::data_source>");
    
    const auto& nested_pair_value_obj = std::get<object>(obj_pair.v.at("value")); // This is the object containing "first" and "second"
    
    // Check "first" element of the pair (std::string)
    const auto& first_elem_typed_summary = std::get<object>(nested_pair_value_obj.v.at("first"));
    EXPECT_EQ(std::get<std::string>(first_elem_typed_summary.v.at("typeid")), "std::string");
    EXPECT_EQ(std::get<std::string>(first_elem_typed_summary.v.at("value")), "key");

    // Check that the nested data_source object is also typed
    const auto& second_elem_typed_summary = std::get<object>(nested_pair_value_obj.v.at("second"));
    EXPECT_EQ(std::get<std::string>(second_elem_typed_summary.v.at("typeid")), "docwire::data_source");
    
    const auto& nested_ds_value_obj = std::get<object>(second_elem_typed_summary.v.at("value")); // This is the object containing "path" and "file_extension" from the nested data_source
    
    // Check path within the nested data_source
    const auto& nested_path_typed_summary = std::get<object>(nested_ds_value_obj.v.at("path"));
    EXPECT_EQ(std::get<std::string>(nested_path_typed_summary.v.at("typeid")), "std::optional<std::filesystem::path>");
    const auto& nested_path_value_obj = std::get<object>(nested_path_typed_summary.v.at("value"));
    EXPECT_EQ(std::get<std::string>(nested_path_value_obj.v.at("typeid")), "std::filesystem::path");
    EXPECT_EQ(std::get<std::string>(nested_path_value_obj.v.at("value")), "test.txt");

    // Check file_extension within the nested data_source
    const auto& nested_fe_typed_summary = std::get<object>(nested_ds_value_obj.v.at("file_extension"));
    EXPECT_EQ(std::get<std::string>(nested_fe_typed_summary.v.at("typeid")), "std::optional<docwire::file_extension>");
    const auto& nested_fe_value_obj = std::get<object>(nested_fe_typed_summary.v.at("value"));
    EXPECT_EQ(std::get<std::string>(nested_fe_value_obj.v.at("typeid")), "docwire::file_extension");
    const auto& nested_fe_inner_value_obj = std::get<object>(nested_fe_value_obj.v.at("value"));
    EXPECT_EQ(std::get<std::string>(nested_fe_inner_value_obj.v.at("value")), ".txt");
}

TEST(Serialization, Concepts)
{
    using namespace docwire::serialization;

    // Test value_alternative for types that ARE in the variant
    static_assert(value_alternative<std::nullptr_t>);
    static_assert(value_alternative<bool>);
    static_assert(value_alternative<std::int64_t>);
    static_assert(value_alternative<std::uint64_t>);
    static_assert(value_alternative<double>);
    static_assert(value_alternative<std::string>);
    static_assert(value_alternative<array>);
    static_assert(value_alternative<object>);

    // Test value_alternative for types that are NOT in the variant
    static_assert(!value_alternative<int>);
    static_assert(!value_alternative<float>);
    static_assert(!value_alternative<char>);
    static_assert(!value_alternative<const char*>);
    static_assert(!value_alternative<std::vector<int>>);
    static_assert(!value_alternative<docwire::data_source>);

    // Test the general variant_alternative with a custom variant
    using my_variant = std::variant<int, float, std::string>;
    static_assert(variant_alternative<int, my_variant>);
    static_assert(!variant_alternative<double, my_variant>);
}

TEST(Serialization, PrettyName)
{
    EXPECT_EQ(type_name::pretty<int>(), "int");
    EXPECT_EQ(type_name::pretty<const int&>(), "const int&");
    EXPECT_EQ(type_name::pretty<std::string>(), "std::string");
    EXPECT_EQ(type_name::pretty<const std::string>(), "const std::string");
    EXPECT_EQ((type_name::pretty<std::pair<int, float>>()), "std::pair<int,float>");
    EXPECT_EQ((type_name::pretty<std::pair<std::string, docwire::data_source&&>>()), "std::pair<std::string,docwire::data_source&&>");
    EXPECT_EQ(type_name::pretty<const char*>(), "const char*");
    EXPECT_EQ((type_name::pretty<std::pair<int, std::pair<double, std::string>>>()), "std::pair<int,std::pair<double,std::string>>");
}

struct int_alias { int v; };
struct string_alias { std::string v; };

TEST(Serialization, StrongTypeAlias)
{
    using namespace docwire::serialization;

    // Test pure serialization (full)
    int_alias my_int{123};
    value v_full = full(my_int);
    ASSERT_TRUE(std::holds_alternative<std::int64_t>(v_full));
    EXPECT_EQ(std::get<std::int64_t>(v_full), 123);

    // Test typed summary
    value v_typed = typed_summary(my_int);
    ASSERT_TRUE(std::holds_alternative<object>(v_typed));
    const auto& obj_typed = std::get<object>(v_typed);
    EXPECT_EQ(std::get<std::string>(obj_typed.v.at("typeid")), "int_alias");
    EXPECT_EQ(std::get<std::int64_t>(obj_typed.v.at("value")), 123);
}

/*
// This test demonstrates the static_assert for missing serializers.
// It is commented out because it is designed to fail compilation.
TEST(Serialization, StaticAssertForMissingSerializer)
{
    struct MyCustomTypeWithoutSerializer {};

    // The following line will fail to compile with a clear static_assert message:
    // "docwire::serialization::serializer<T> is not specialized for this type.
    //  Please provide a specialization for your type `T`."
    //
    // docwire::serialization::value v = docwire::serialization::full(MyCustomTypeWithoutSerializer{});
}
*/

TEST(Stringification, StrongTypeAlias)
{
    int_alias my_int{123};
    EXPECT_EQ(stringify(my_int), "123");

    string_alias my_str{"hello"};
    EXPECT_EQ(stringify(my_str), "hello");
}

TEST(stringification, enums)
{
    ASSERT_EQ(stringify(confidence::very_high), "very_high");
}
