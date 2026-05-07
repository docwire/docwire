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

#include "content_type_by_file_extension.h"
#include "content_type_by_signature.h"
#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"
#include "data_source.h"
#include "diagnostic_message.h"
#include "file_extension.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

using namespace docwire;

TEST(content_type, by_file_extension)
{
    data_source data { std::filesystem::path{"1.docx"} };
    try {
        content_type::by_file_extension::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::ElementsAre(
        std::pair {
            mime_type { "application/vnd.openxmlformats-officedocument.wordprocessingml.document" },
            confidence::high
        }
    ));
}

TEST(content_type, to_extension)
{
    using namespace docwire::content_type::by_file_extension;
    using docwire::mime_type;

    // Test a common case where multiple extensions exist for one mime type.
    // The first one in the list should be chosen, which is .txt for text/plain.
    auto txt_ext = to_extension(mime_type{"text/plain"});
    ASSERT_TRUE(txt_ext.has_value());
    ASSERT_EQ(txt_ext->string(), ".txt");

    // Test a standard, unambiguous mime type.
    auto pdf_ext = to_extension(mime_type{"application/pdf"});
    ASSERT_TRUE(pdf_ext.has_value());
    ASSERT_EQ(pdf_ext->string(), ".pdf");

    // Test another common office format.
    auto docx_ext = to_extension(mime_type{"application/vnd.openxmlformats-officedocument.wordprocessingml.document"});
    ASSERT_TRUE(docx_ext.has_value());
    ASSERT_EQ(docx_ext->string(), ".docx");

    // Test a non-existent mime type.
    auto non_existent_ext = to_extension(mime_type{"application/x-non-existent"});
    ASSERT_FALSE(non_existent_ext.has_value());
}

TEST(content_type, by_signature)
{
    auto prepare_data = []()
    {
        return data_source { seekable_stream_ptr { std::make_shared<std::ifstream>("1.doc", std::ios_base::binary) } };
    };
    auto check_result = [](const data_source& data)
    {
        using namespace testing;
        ASSERT_THAT(data.mime_types, testing::ElementsAre(
            std::pair {
                mime_type { "application/msword" },
                confidence::very_high
            }
        ));
    };
    try {
        data_source data = prepare_data();
        content_type::by_signature::detect(data);
        check_result(data);
        content_type::by_signature::database db;
        data = prepare_data();
        content_type::by_signature::detect(data, db);
        check_result(data);
        data = prepare_data();
        content_type::by_signature::detect(data, db);
        check_result(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
}

TEST(content_type, html)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.html", std::ios_base::binary) }};
    try {
        content_type::html::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::ElementsAre(
        std::pair {
            mime_type { "text/html" },
            confidence::highest
        }
    ));
}

TEST(content_type, iwork)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.key", std::ios_base::binary) }};
    try {
        content_type::iwork::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::ElementsAre(
        std::pair {
            mime_type { "application/vnd.apple.keynote" },
            confidence::highest
        }
    ));
}

TEST(content_type, odf_flat)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.fods", std::ios_base::binary) }};
    try {
        content_type::odf_flat::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::ElementsAre(
        std::pair {
            mime_type { "application/vnd.oasis.opendocument.spreadsheet-flat-xml" },
            confidence::highest
        }
    ));
}

TEST(content_type, outlook)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.pst", std::ios_base::binary) }};
    try {
        content_type::outlook::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::UnorderedElementsAre(
        std::pair {
            mime_type { "application/vnd.ms-outlook-pst" },
            confidence::highest
        },
        std::pair {
            mime_type { "application/vnd.ms-outlook" },
            confidence::very_high
        },
        std::pair {
            mime_type { "application/octet-stream" },
            confidence::very_high
        }
    ));
}

TEST(content_type, xlsb)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.xlsb", std::ios_base::binary) }};
    try {
        content_type::xlsb::detect(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    using namespace testing;
    ASSERT_THAT(data.mime_types, testing::ElementsAre(
        std::pair {
            mime_type { "application/vnd.ms-excel.sheet.binary.macroenabled.12" },
            confidence::highest
        }
    ));
}
