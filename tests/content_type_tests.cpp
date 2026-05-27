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

#include "content_type.h"
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
#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

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

// ============================================================================
// COMPREHENSIVE PARAMETERIZED TESTS
// ============================================================================

struct ContentTypeTestCase {
    std::string file_name;
    std::vector<std::string> expected_signature_mimes; // What libmagic alone sees (allows multiple valid outputs)
    std::string expected_final_mime;     // What the pipeline MUST resolve
};

void PrintTo(const ContentTypeTestCase& test_case, std::ostream* os) {
    *os << "{ file_name: \"" << test_case.file_name
        << "\", expected_signature_mimes: {";
    for (size_t i = 0; i < test_case.expected_signature_mimes.size(); ++i) {
        *os << (i == 0 ? "" : ", ") << "\"" << test_case.expected_signature_mimes[i] << "\"";
    }
    *os << "}, expected_final_mime: \"" << test_case.expected_final_mime << "\" }";
}

// Generates clean names like "1_docx" instead of "0" or "1.docx"
static std::string GenerateTestName(const ::testing::TestParamInfo<ContentTypeTestCase>& info) {
    std::string name = info.param.file_name;
    std::replace(name.begin(), name.end(), '.', '_');
    std::replace(name.begin(), name.end(), '-', '_');
    return name;
}

static const std::vector<ContentTypeTestCase> content_type_test_cases = {
    // Modern Office Formats
    {"1.docx", {"application/zip"}, "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {"1.xlsx", {"application/zip"}, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {"1.pptx", {"application/zip"}, "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    
    // Legacy Office Formats
    {"1.doc", {"application/msword"}, "application/msword"},
    {"1.xls", {"application/vnd.ms-excel"}, "application/vnd.ms-excel"},
    {"1.ppt", {"application/vnd.ms-powerpoint"}, "application/vnd.ms-powerpoint"},
    
    // Formats with Custom Detectors
    {"1.xlsb", {"application/zip"}, "application/vnd.ms-excel.sheet.binary.macroenabled.12"},
    {"1.key", {"application/zip"}, "application/vnd.apple.keynote"},
    {"1.pages", {"application/zip"}, "application/vnd.apple.pages"},
    {"1.numbers", {"application/zip"}, "application/vnd.apple.numbers"},
    {"1.pst", {"application/vnd.ms-outlook"}, "application/vnd.ms-outlook-pst"},
    {"1.html", {"text/html", "application/xhtml+xml"}, "text/html"},
    {"test.asp", {"text/html"}, "text/asp"},
    {"test.aspx", {"text/html"}, "text/aspdotnet"},
    
    // ODF Formats
    {"1.odt", {"application/vnd.oasis.opendocument.text", "application/zip"}, "application/vnd.oasis.opendocument.text"},
    {"1.ods", {"application/vnd.oasis.opendocument.spreadsheet", "application/zip"}, "application/vnd.oasis.opendocument.spreadsheet"},
    {"1.odp", {"application/vnd.oasis.opendocument.presentation", "application/zip"}, "application/vnd.oasis.opendocument.presentation"},
    
    // ODF Flat XML Formats
    {"1.fodt", {"application/xml"}, "application/vnd.oasis.opendocument.text-flat-xml"},
    {"1.fods", {"application/xml"}, "application/vnd.oasis.opendocument.spreadsheet-flat-xml"},
    {"1.fodp", {"application/xml"}, "application/vnd.oasis.opendocument.presentation-flat-xml"},
    
    // Archives
    {"test.zip", {"application/zip"}, "application/zip"},
    {"test.tar", {"application/x-tar"}, "application/x-tar"},
    {"test.rar", {"application/vnd.rar"}, "application/vnd.rar"},
    {"test.tar.gz", {"application/gzip"}, "application/gzip"},
    {"test.tar.bz2", {"application/x-bzip2"}, "application/x-bzip2"},
    {"test.tar.xz", {"application/x-xz"}, "application/x-xz"},
    
    // Images
    {"basic_ocr-eng.png", {"image/png"}, "image/png"},
    {"basic_ocr-eng.jpg", {"image/jpeg"}, "image/jpeg"},
    {"basic_ocr-eng.tiff", {"image/tiff"}, "image/tiff"},
    {"basic_ocr-eng.bmp", {"image/bmp", "application/octet-stream"}, "image/bmp"},
    {"basic_ocr-eng.webp", {"image/webp", "application/octet-stream"}, "image/webp"},

    // Other formats
    {"1.pdf", {"application/pdf"}, "application/pdf"},
    {"1.rtf", {"application/rtf"}, "application/rtf"},
    {"first.eml", {"message/rfc822"}, "message/rfc822"},
    {"test.xml", {"application/xml"}, "application/xml"},
    {"test.json", {"application/json"}, "application/json"}
};

// ============================================================================
// TEST SUITE 1: content_type::by_signature
// ============================================================================
class by_signature : public ::testing::TestWithParam<ContentTypeTestCase> {
protected:
    static std::shared_ptr<content_type::by_signature::database> db;
    static void SetUpTestSuite() { db = std::make_shared<content_type::by_signature::database>(); }
    static void TearDownTestSuite() { db.reset(); }
};
std::shared_ptr<content_type::by_signature::database> by_signature::db = nullptr;

TEST_P(by_signature, detect)
{
    const auto& param = GetParam();
    std::ifstream ifs(param.file_name, std::ios_base::binary);
    ASSERT_TRUE(ifs.good()) << "Test file " << param.file_name << " not found.";
    
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>(std::move(ifs)) } };
    content_type::by_signature::detect(data, *db);
    
    auto highest = data.highest_confidence_mime_type();
    ASSERT_TRUE(highest.has_value()) << "No MIME type detected by libmagic.";
    
    bool is_expected = std::find(param.expected_signature_mimes.begin(), 
                                 param.expected_signature_mimes.end(), 
                                 highest->v) != param.expected_signature_mimes.end();
                                 
    EXPECT_TRUE(is_expected) 
        << "libmagic returned '" << highest->v 
        << "', which is not in the list of expected variants for " << param.file_name;
}

// This generates: content_type/by_signature.detect/1_docx
INSTANTIATE_TEST_SUITE_P(content_type, by_signature, ::testing::ValuesIn(content_type_test_cases), GenerateTestName);


// ============================================================================
// TEST SUITE 2: content_type::detector (Top-level)
// ============================================================================
class detector : public ::testing::TestWithParam<ContentTypeTestCase> {
protected:
    static std::shared_ptr<content_type::by_signature::database> db;
    static void SetUpTestSuite() { db = std::make_shared<content_type::by_signature::database>(); }
    static void TearDownTestSuite() { db.reset(); }
};
std::shared_ptr<content_type::by_signature::database> detector::db = nullptr;

TEST_P(detector, detect_from_path)
{
    const auto& param = GetParam();
    data_source data { std::filesystem::path{param.file_name} };
    
    content_type::detect(data, *db);
    // contains_type::detect(data); // Uncomment if needed for OOXML stream resolution
    
    auto highest = data.highest_confidence_mime_type();
    ASSERT_TRUE(highest.has_value()) << "No MIME type detected.";
    EXPECT_EQ(highest->v, param.expected_final_mime);
}

TEST_P(detector, detect_from_stream)
{
    const auto& param = GetParam();
    std::ifstream ifs(param.file_name, std::ios_base::binary);
    ASSERT_TRUE(ifs.good()) << "Test file " << param.file_name << " not found.";
    
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>(std::move(ifs)) } };
    
    content_type::detect(data, *db);
    // contains_type::detect(data); // Uncomment if needed for OOXML stream resolution
    
    auto highest = data.highest_confidence_mime_type();
    ASSERT_TRUE(highest.has_value()) << "No MIME type detected.";
    EXPECT_EQ(highest->v, param.expected_final_mime);
}

// This generates: content_type/detector.detect_from_path/1_docx
// And:            content_type/detector.detect_from_stream/1_docx
INSTANTIATE_TEST_SUITE_P(content_type, detector, ::testing::ValuesIn(content_type_test_cases), GenerateTestName);
