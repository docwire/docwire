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

#include "base64.h"
#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <boost/json.hpp>
#include "convert_chrono.h" // IWYU pragma: keep
#include "error_hash.h" // IWYU pragma: keep
#include "fuzzy_match.h"
#include "gtest/gtest.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include "serialization_document_elements.h" // IWYU pragma: keep

using namespace docwire;

TEST(fuzzy_match, ratio)
{
    ASSERT_EQ(docwire::fuzzy_match::ratio("hello", "hello"), 100.0);
    ASSERT_EQ(docwire::fuzzy_match::ratio("hello", "helll"), 80.0);
}

TEST(base64, encode)
{
    const std::string input_str { "test" };
    const std::span<const std::byte> input_data { reinterpret_cast<const std::byte*>(input_str.c_str()), input_str.size() };
    std::string encoded = base64::encode(input_data);
    ASSERT_EQ(encoded, "dGVzdA==");
}

TEST(base64, decode)
{
    const std::string input_str { "dGVzdA==" };
    std::vector<std::byte> decoded = base64::decode(input_str);
    std::string decoded_str { reinterpret_cast<char*>(decoded.data()), decoded.size() };
    ASSERT_EQ(decoded_str, "test");
}
