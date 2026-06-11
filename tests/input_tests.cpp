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

#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <boost/json.hpp>
#include "content_type_by_file_extension.h"
#include "content_type_by_signature.h"
#include "data_source.h"
#include "gtest/gtest.h"
#include <string_view>
#include <fstream>
#include <iterator>
#include <magic_enum/magic_enum_iostream.hpp>
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "input.h"

using namespace docwire;

namespace
{
std::string read_test_file(const std::string& file_name)
{
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream.open(file_name);
    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

std::string read_binary_file(const std::string& file_name)
{
    std::ifstream stream;
    stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream.open(file_name, std::ios::binary);
    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}
}

TEST(Input, data_source_with_file_ext)
{
    std::ostringstream output_stream{};
    data_source{seekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios::binary)}, file_extension{".doc"}} |
        content_type::by_file_extension::detector{} |
        office_formats_parser{} |
        PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, path_ref)
{
    std::ostringstream output_stream{};
    std::filesystem::path path{"1.doc"};
    path | content_type::by_file_extension::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, path_temp)
{
    std::ostringstream output_stream{};    
    std::filesystem::path{"1.doc"} | content_type::by_file_extension::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, vector_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::vector<std::byte> vector{reinterpret_cast<const std::byte*>(str.data()), reinterpret_cast<const std::byte*>(str.data()) + str.size()};
    vector | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, vector_temp)
{
    std::ostringstream output_stream{};    
    std::string str = read_binary_file("1.doc");
    std::vector<std::byte>{reinterpret_cast<const std::byte*>(str.data()), reinterpret_cast<const std::byte*>(str.data()) + str.size()} |
        content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, span_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::span<const std::byte> span{reinterpret_cast<const std::byte*>(str.data()), str.size()};
    span | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, span_temp)
{
    std::ostringstream output_stream{};    
    std::string str = read_binary_file("1.doc");
    std::span<const std::byte>{reinterpret_cast<const std::byte*>(str.data()), str.size()} |
        content_type::by_signature::detector{} |
        office_formats_parser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    str | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_temp)
{
    std::ostringstream output_stream{};    
    read_binary_file("1.doc") | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_view_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::string_view string_view{str};
    string_view | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_view_temp)
{
    std::ostringstream output_stream{};    
    std::string input_str = read_binary_file("1.doc");
    std::string_view{input_str} | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} |
        content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} |
        content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_shared_ptr)
{
    std::ostringstream output_stream{};
    std::make_shared<std::ifstream>("1.doc", std::ios_base::binary) |
        content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_temp)
{
    std::ostringstream output_stream{};
    std::ifstream{"1.doc", std::ios_base::binary} |
        content_type::by_signature::detector{} |
        DOCParser{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}
