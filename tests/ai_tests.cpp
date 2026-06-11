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
#include "diagnostic_message.h"
#include <exception>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include "resource_path.h"
#include "tokenizer.h"

using namespace docwire;

TEST(tokenizer, flan_t5)
{
    docwire::ai::ct2::tokenizer tokenizer { resource_path("flan-t5-large-ct2-int8") };

    // Test case for an empty input string. It should return only the end of sequence token.
    ASSERT_THAT(tokenizer.tokenize(""),
        ::testing::ElementsAre("</s>"));
    ASSERT_THAT(tokenizer.encode(""),
        ::testing::ElementsAre(1));
    ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{""}), "");

    // Simple case with common words.
    ASSERT_THAT(tokenizer.tokenize("test input"),
        ::testing::ElementsAre("▁test", "▁input", "</s>"));
    ASSERT_THAT(tokenizer.encode("test input"),
        ::testing::ElementsAre(794, 3785, 1));
    // Detokenization should correctly join the tokens back into the original string.
    ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{"▁test", "▁input"}), "test input");

    // A more complex sentence to show subword tokenization.
    // "Tokenization" is likely not in the vocabulary as a single unit and will be split.
    // The exact tokenization depends on the vocabulary learned in spiece.model.
    ASSERT_THAT(tokenizer.tokenize("Tokenization is useful."),
        ::testing::ElementsAre("▁To", "ken", "ization", "▁is", "▁useful", ".", "</s>"));
    ASSERT_THAT(tokenizer.encode("Tokenization is useful."),
        ::testing::ElementsAre(304, 2217, 1707, 19, 1934, 5, 1));
    ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{"▁To", "ken", "ization", "▁is", "▁useful", "."}), "Tokenization is useful.");
}

TEST(tokenizer, multilingual_e5)
{
    try
    {
        docwire::ai::ct2::tokenizer tokenizer { resource_path("multilingual-e5-small-ct2-int8") };

        // Test case for an empty input string. It should return only the end of sequence token.
        ASSERT_THAT(tokenizer.tokenize(""),
            ::testing::ElementsAre("<s>", "</s>"));
        ASSERT_THAT(tokenizer.encode(""),
            ::testing::ElementsAre(0, 2));
        ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{""}), "");

        // Simple case with common words.
        ASSERT_THAT(tokenizer.tokenize("test input"),
            ::testing::ElementsAre("<s>", "▁test", "▁input", "</s>"));
        ASSERT_THAT(tokenizer.encode("test input"),
            ::testing::ElementsAre(0, 3034, 107730, 2));
        // Detokenization should correctly join the tokens back into the original string.
        ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{"▁test", "▁input"}), "test input");

        // A more complex sentence to show subword tokenization.
        // "Tokenization" is likely not in the vocabulary as a single unit and will be split.
        // The exact tokenization depends on the vocabulary learned in spiece.model.
        ASSERT_THAT(tokenizer.tokenize("Tokenization is useful."),
            ::testing::ElementsAre("<s>", "▁To", "ken", "ization", "▁is", "▁useful", ".", "</s>"));
        ASSERT_THAT(tokenizer.encode("Tokenization is useful."),
            ::testing::ElementsAre(0, 717, 1098, 47691, 83, 80234, 5, 2));
        ASSERT_EQ(tokenizer.detokenize(std::vector<std::string>{"▁To", "ken", "ization", "▁is", "▁useful", "."}), "Tokenization is useful.");
    }
    catch (const std::exception& e)
    {
        FAIL() << errors::diagnostic_message(e);
    }
}
