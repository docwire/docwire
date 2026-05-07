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

#include "archives_parser.h"
#include "eml_parser.h"
#include "pst_parser.h"
#include "html_parser.h"
#include "message_matchers.h" // IWYU pragma: keep
#include "pdf_parser.h"
#include "content_type.h"
#include <filesystem>

using namespace docwire;
using namespace testing;

// Helper for direct parser testing
template <typename Parser>
void TestParserFailures(const std::string& filename,
                          std::function<void(message_ptr)> back_inspector,
                          std::function<void(message_ptr)> forward_inspector)
{
    data_source ds{std::filesystem::path{filename}};
    // Run detector to populate mime types
    content_type::detect(ds);

    Parser parser;
    parser(std::make_shared<message<data_source>>(std::move(ds)), {
        [&](message_ptr msg) {
            if (forward_inspector) forward_inspector(msg);
            return continuation::proceed;
        },
        [&](message_ptr msg) {
            if (back_inspector) back_inspector(msg);
            return continuation::proceed;
        }
    });
}

TEST(PartialAndTotalFailures, Zip_Partial)
{
    int count = 0;
    bool error_received = false;
    TestParserFailures<archives_parser>("test.zip",
        [&](message_ptr msg) {
            if (msg->is<data_source>()) {
                count++;
                if (count == 1) throw std::runtime_error("Simulated failure");
            }
        },
        [&](message_ptr msg) {
            if (msg->is<std::exception_ptr>()) error_received = true;
        }
    );
    // Should not throw, meaning resilience worked.
    EXPECT_GT(count, 1);
    EXPECT_TRUE(error_received);
}

TEST(PartialAndTotalFailures, Zip_Total)
{
    EXPECT_THROW({
        TestParserFailures<archives_parser>("test.zip",
            [](message_ptr msg) {
                if (msg->is<data_source>()) throw std::runtime_error("Simulated failure");
            },
            nullptr
        );
    }, std::exception);
}

TEST(PartialAndTotalFailures, EML_Partial)
{
    int count = 0;
    bool body_received = false;
    bool error_received = false;
    TestParserFailures<EMLParser>("fourth.eml",
        [&](message_ptr msg) {
            if (msg->is<data_source>()) {
                count++;
                // fourth.eml has 1 attachment (data_source) and body (document::Text)
                if (count == 1) throw std::runtime_error("Simulated attachment failure");
            }
        },
        [&](message_ptr msg) {
            if (msg->is<document::Text>()) body_received = true;
            if (msg->is<std::exception_ptr>()) error_received = true;
        }
    );
    EXPECT_TRUE(body_received);
    EXPECT_TRUE(error_received);
    EXPECT_EQ(count, 1);
}

TEST(PartialAndTotalFailures, EML_Total)
{
    EXPECT_THROW({
        TestParserFailures<EMLParser>("fourth.eml",
            [](message_ptr msg) {
                if (msg->is<data_source>()) throw std::runtime_error("Simulated failure");
            },
            [](message_ptr msg) {
                if (msg->is<document::Text>()) throw std::runtime_error("Simulated failure");
            }
        );
    }, std::exception);
}

TEST(PartialAndTotalFailures, PST_Partial)
{
    int count = 0;
    bool error_received = false;
    TestParserFailures<PSTParser>("1.pst",
        [&](message_ptr msg) {
            if (msg->is<data_source>()) {
                count++;
                if (count == 1) throw std::runtime_error("Simulated failure");
            }
        },
        [&](message_ptr msg) {
            if (msg->is<std::exception_ptr>()) error_received = true;
        }
    );
    // Should not throw, meaning resilience worked.
    EXPECT_GT(count, 1);
    EXPECT_TRUE(error_received);
}

TEST(PartialAndTotalFailures, PST_Total)
{
    EXPECT_THROW({
        TestParserFailures<PSTParser>("1.pst",
            [](message_ptr msg) {
                if (msg->is<data_source>()) throw std::runtime_error("Simulated failure");
            },
            [](message_ptr msg) {
                if (!msg->is<document::Document>() && !msg->is<document::CloseDocument>())
                    throw std::runtime_error("Simulated failure");
            }
        );
    }, std::exception);
}

TEST(PartialAndTotalFailures, HTML_Partial)
{
    int count = 0;
    std::string text_output;
    bool error_received = false;
    TestParserFailures<HTMLParser>("embedded_images.html",
        // back inspector
        [&](message_ptr msg) {
            if (msg->is<document::Image>()) {
                count++;
                if (count == 1) throw std::runtime_error("Simulated image failure");
            }
        },
        // forward inspector
        [&](message_ptr msg) {
            if (msg->is<document::Text>()) {
                text_output += msg->get<document::Text>().text;
            } else if (msg->is<std::exception_ptr>()) {
                error_received = true;
            }
        }
    );
    EXPECT_THAT(text_output, HasSubstr("and the second image:"));
    EXPECT_TRUE(error_received);
    EXPECT_GT(count, 1);
}

TEST(PartialAndTotalFailures, HTML_Total)
{
    EXPECT_THROW({
        TestParserFailures<HTMLParser>("embedded_images.html",
            [](message_ptr msg) {
                throw std::runtime_error("Simulated image failure");
            },
            [](message_ptr msg) {
                if (!msg->is<document::Document>() && !msg->is<document::CloseDocument>())
                    throw std::runtime_error("Simulated text failure");
            }
        );
    }, std::exception);
}

TEST(PartialAndTotalFailures, PDF_Partial)
{
    int count = 0;
    std::string text_output;
    bool error_received = false;
    TestParserFailures<PDFParser>("embedded_images.pdf",
        [&](message_ptr msg) {
            if (msg->is<document::Image>()) {
                count++;
                if (count == 1) throw std::runtime_error("Simulated image failure");
            }
        },
        [&](message_ptr msg) {
            if (msg->is<document::Text>()) {
                text_output += msg->get<document::Text>().text;
            } else if (msg->is<std::exception_ptr>()) {
                error_received = true;
            }
        }
    );
    EXPECT_THAT(text_output, HasSubstr("and the second image:"));
    EXPECT_TRUE(error_received);
    EXPECT_GT(count, 1);
}

TEST(PartialAndTotalFailures, PDF_Total)
{
    EXPECT_THROW({
        TestParserFailures<PDFParser>("embedded_images.pdf",
            [](message_ptr msg) {
                if (msg->is<document::Image>()) throw std::runtime_error("Simulated image failure");
            },
            [](message_ptr msg) {
                if (msg->is<document::Text>()) throw std::runtime_error("Simulated text failure");
            }
        );
    }, std::exception);
}
