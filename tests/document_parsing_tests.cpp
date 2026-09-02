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
#include "contains_type.h"
#include "content_type.h"
#include "content_type_by_file_extension.h"
#include "diagnostic_message.h"
#include "error_tags.h"
#include <exception>
#include <future>
#include "gtest/gtest.h"
#include <string_view>
#include <tuple>
#include "archives_parser.h"
#include <fstream>
#include "html_exporter.h"
#include "language.h"
#include <iterator>
#include <array>
#include <magic_enum/magic_enum_iostream.hpp>
#include "mail_parser.h"
#include "meta_data_exporter.h"
#include "standard_filter.h"
#include <optional>
#include <algorithm>
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "transformer_func.h"
#include "input.h"
#include "log.h"

using namespace docwire;

std::vector<std::string> generate_list_for_basic_tests() {
    std::vector<std::string> files;
    std::vector<std::tuple<int, const char*>> ranges_formats = {
        {9, "odt"}, {9, "fodt"}, {9, "ods"}, {9, "fods"}, {9, "odp"}, {9, "fodp"},
        {9, "odg"}, {9, "fodg"}, {9, "rtf"}, {9, "doc"}, {9, "xls"}, {9, "xlsb"},
        {9, "pdf"}, {9, "ppt"}, {9, "docx"}, {9, "xlsx"}, {9, "pptx"},
        {9, "pages"}, {9, "numbers"}, {9, "key"}, {9, "html"}, {1, "pst"}
    };
    for (const auto& rf_tuple : ranges_formats) {
        int upper_bound = std::get<0>(rf_tuple);
        const char* format = std::get<1>(rf_tuple);
        for (int i = 1; i <= upper_bound; ++i) {
            files.push_back(std::to_string(i) + "." + format);
        }
    }
    return files;
}

std::vector<std::string> get_misc_test_files_list() {
    return {
        "biff5.xls", "hebrew_1.rtf", "hebrew_2.rtf", "japanese.xls",
        "embedded_spreadsheet.doc", "encoding_in_table.doc", "fields.doc",
        "fields.docx", "crash_test.xls", "special_para_cmds.rtf",
        "header_footer.doc", "shared_strings.xls", "shared_strings.xlsb",
        "whitespaces.odt", "whitespaces.doc", "whitespaces.docx",
        "space_attributes_in_non_standard_places.docx", "ole.odt", "ole.fodt",
        "nested_tables.html", "first.eml", "second.eml", "third.eml", "fourth.eml",
        "simple_text.eml", "multipart_alternative.eml", "base64_text.eml", "nested_multipart.eml",
        "html_priority.eml", "empty_html_fallback.eml", "nested_html.eml",
        "alternative_plain_only.eml", "boundary_with_dashes.eml",
        "unclosed_inner_boundary.eml", "nested_deep_unclosed.eml",
        "header_folded_boundary.eml", "unnamed_attachment.eml",
        "multipart_related_html.eml", "html_attachment_alternative.eml",
        "endboundary_first.eml", "missing_inner_closing_boundary.eml",
        "nested_multiparts_missing.eml", "valid_format.eml", "no_multipart_start.eml",
        "html_with_doc_ext.doc", "html_with_xls_ext.xls", "rtf_with_doc_ext.doc",
        "comments_libreoffice_3.5.odt", "comments_libreoffice_3.5.doc",
        "comments_libreoffice_3.5.docx", "comments_libreoffice_3.5.rtf",
        "gb18030_without_bom.text", "utf16_with_bom.text", "test.asp", "test.aspx",
        "test.css", "test.fpp", "test.js", "test.json", "test.txt", "test.xml",
        "empty_cells.xlsx", "test.zip", "test.tar", "test.tar.gz", "test.tar.bz2",
        "test.tar.xz", "test.rar", "basic_ocr-eng.bmp", "basic_ocr-eng.jpg",
        "basic_ocr-eng.jpeg", "basic_ocr-eng.png", "basic_ocr-eng.tiff",
        "basic_ocr-eng.webp", "75dpi-eng.png", "white_on_black-pol.png",
        "diacritical_marks-pol.png", "paragraphs-eng.png",
        "multilang-chi_sim-fra-deu-eng.png", "multilang-chi_tra-rus-jpn.png",
        "multilang-spa-lat-grc.png", "multilang-hin-san-swa-kor-eng.png",
        "embedded_images.pdf", "embedded_images.html"
    };
}

class document_parsing_tests : public ::testing::TestWithParam<std::string> {
protected:
    std::vector<language> get_ocr_languages_from_filename(const std::string& file_name) const {
        std::vector<language> langs;
        if (file_name.length() > 4 &&
            (boost::algorithm::ends_with(file_name, ".png") ||
             boost::algorithm::ends_with(file_name, ".bmp") ||
             boost::algorithm::ends_with(file_name, ".jpg") ||
             boost::algorithm::ends_with(file_name, ".jpeg") ||
             boost::algorithm::ends_with(file_name, ".tiff") ||
             boost::algorithm::ends_with(file_name, ".webp")))
        {
            std::vector<std::string> fn_parts;
            boost::split(fn_parts, file_name, boost::is_any_of("-."));
            for (const std::string& fn_part : fn_parts) {
                std::string_view fn_part_view = fn_part;
                std::optional<language> lang = magic_enum::enum_cast<language>(fn_part_view, magic_enum::case_insensitive);
                if (lang) {
                    langs.push_back(*lang);
                }
            }
        }
        if (langs.empty()) {
            langs.push_back(language::pol);
        }
        return langs;
    }

    template <typename ChainStart>
    void ParseAndAssertOutput(const std::string& file_name,
                              ChainStart&& chain_start_obj) {
        std::ifstream expected_ifs{ file_name + ".out" };
        ASSERT_TRUE(expected_ifs.good()) << "Expected output file " << file_name << ".out" << " not found.\n";
        std::string expected_text{ std::istreambuf_iterator<char>{expected_ifs},
                                   std::istreambuf_iterator<char>{}};

        std::vector<language> ocr_langs = get_ocr_languages_from_filename(file_name);

        std::ostringstream output_stream{};
        try {
            std::forward<ChainStart>(chain_start_obj) |
                content_type::detector{} |
                archives_parser{} |
                office_formats_parser{} | mail_parser{} | ocr_parser{ocr_langs} |
                plain_text_exporter() |
                output_stream;
        } catch (const std::exception& e) {
            FAIL() << errors::diagnostic_message(e);
        }
        // THEN
        EXPECT_EQ(expected_text, output_stream.str());
    }
};

TEST_P(document_parsing_tests, ParseFromPathAndExtractText) {
    // GIVEN
    const std::string& file_name = GetParam();

    SCOPED_TRACE("file_name (path) = " + file_name);

    // WHEN & THEN
    ParseAndAssertOutput(file_name, std::filesystem::path{file_name});
}

TEST_P(document_parsing_tests, ParseFromStreamAndExtractText) {
    // GIVEN
    const std::string& file_name = GetParam();

    SCOPED_TRACE("file_name (stream) = " + file_name);

    // WHEN & THEN
    ParseAndAssertOutput(file_name, std::ifstream(file_name, std::ios_base::binary));
}

void escape_test_name(std::string& str)
{
    std::transform(str.cbegin(), str.cend(), str.begin(), [](const auto ch)
        {   if(ch == '.') return '_';
            else if(ch == '-') return '_';
            else return ch;
        }
    );
}

class metadata_test : public ::testing::TestWithParam<const char*>
{
protected:

    static constexpr std::array<std::string_view, 2> names
    {
        "meta_libreoffice_3.5_created",
        "meta_libreoffice_3.5_modified"
    };
};

TEST_P(metadata_test, ParseFromPathTest)
{
    auto format = GetParam();

    for(auto name : names)
    {
        // GIVEN
        std::string file_name{ std::string{name.data()} + "." + format };

        std::ifstream ifs{ file_name + ".out" };
        ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";

        std::string expected_text{ std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{}};

        SCOPED_TRACE("file_name = " + file_name);

        // WHEN
        std::ostringstream output_stream{};

        std::filesystem::path{file_name} |
            content_type::by_file_extension::detector{} |
          office_formats_parser{} | mail_parser{} | ocr_parser{} |
          metadata_exporter() |
          output_stream;

        // THEN
        EXPECT_EQ(expected_text, output_stream.str());
    }
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFileMetadataTests, metadata_test,
    ::testing::Values(
        "odt", "ods", "odp", "odg", "rtf", "doc", "xls", "ppt", "docx", "xlsx", "pptx", "html"
                      ),
    [](const ::testing::TestParamInfo<metadata_test::ParamType>& info) {
      std::string name = std::string{ info.param } + "_basic_metadata_tests";
      return name;
    });

class callback_test : public ::testing::TestWithParam<std::tuple<const char*, const char*, message_transform_func>>
{
};


TEST_P(callback_test, ParseFromPathTest)
{
    const auto [name, out_name, callback] = GetParam();

    // GIVEN
    std::string file_name{ name };
    std::string output_name{ out_name };

    std::ifstream ifs{ output_name };
    ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";

    std::string expected_text{ std::istreambuf_iterator<char>{ifs},
        std::istreambuf_iterator<char>{}};

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | ocr_parser{} |
        callback |
        plain_text_exporter() |
        output_stream;

    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
}

INSTANTIATE_TEST_SUITE_P(
    StandardFilterTests, callback_test,
    ::testing::Values(
        std::make_tuple("1.pst", "1.pst.2.out", standard_filter::filterByMailMinCreationTime(1644216799))
                      ));

class html_writer_test : public ::testing::TestWithParam<const char*>
{
};

TEST_P(html_writer_test, ParseFromPathTest)
{
    // GIVEN
    auto name = GetParam();
    std::string file_name{ name };

    std::ifstream ifs{ file_name + ".out.html" };
    ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out.html" << " not found\n";

    std::string expected_text{ std::istreambuf_iterator<char>{ifs},
        std::istreambuf_iterator<char>{}};

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | ocr_parser{} |
        html_exporter() |
        output_stream;

    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFileHTMLWriterTest, html_writer_test,
    ::testing::Values(
        "1.docx", "2.docx", "3.docx", "4.docx", "5.docx", "6.docx", "7.docx", "8.docx", "9.docx", "10.docx",
        "1.doc", "2.doc", "3.doc", "4.doc", "5.doc", "6.doc", "7.doc", "8.doc", "9.doc",
        "1.html", "2.html", "3.html", "4.html", "5.html", "6.html", "7.html", "8.html", "9.html",
        "first.eml", "basic_ocr-eng.png", "paragraphs-eng.png"
                      ),
    [](const ::testing::TestParamInfo<html_writer_test::ParamType>& info) {
        std::string file_name = info.param;
        escape_test_name(file_name);

        std::string name = file_name + "_basic_html_tests";
        return name;
    });

class password_protected_test : public ::testing::TestWithParam<const char*>
{
};

TEST_P(password_protected_test, MajorTestingModule)
{
    // GIVEN
    auto format = GetParam();
    std::string file_name{ std::string{"password_protected."} + format };

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    std::ostringstream output_stream{};

    try
    {
        std::filesystem::path{file_name} |
            content_type::by_file_extension::detector{} |
            office_formats_parser{} | mail_parser{} | ocr_parser{} |
            plain_text_exporter() |
            output_stream;
        FAIL() << "We are not supporting password protected files yet. Why didn\'t we catch exception?\n";
    }
    catch (const std::exception& ex)
    {
        ASSERT_TRUE(errors::contains_type<errors::file_encrypted>(ex))
            << "Thrown exception diagnostic message:\n" << errors::diagnostic_message(ex);
    }
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFilePasswordProtectedTests, password_protected_test,
    ::testing::Values(
        "doc", "docx", "key", "pages", "numbers", "odp", "pdf", "ppt", "pptx", "xls", "xlsb", "xlsx"
                      ),
    [](const ::testing::TestParamInfo<password_protected_test::ParamType>& info) {
        std::string format = info.param;
        std::string name = format + "_password_protected_test";
        return name;
    });

class multithreaded_test : public ::testing::TestWithParam<std::tuple<int, int, const char*>>
{
};

void thread_func(const std::string& file_name)
{
    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
      content_type::by_file_extension::detector{} |
      office_formats_parser{} | mail_parser{} | ocr_parser{} |
      plain_text_exporter() |
      output_stream;

    std::filesystem::path{file_name} |
      content_type::by_file_extension::detector{} |
      office_formats_parser{} | mail_parser{} | ocr_parser{} |
      metadata_exporter() |
      output_stream;
}

TEST_P(multithreaded_test, ReadFromFileTests)
{
    const auto [lower, upper, format] = GetParam();

    std::vector<std::future<void>> threads;

    std::vector<std::string> file_names(upper - lower + 1);

    std::generate(file_names.begin(), file_names.end(), [format_str = format, i = lower]() mutable
    {
        return std::to_string(i++) + "." + format_str;
    });

    for(const auto& file_name : file_names)
    {
        // GIVEN
        SCOPED_TRACE("file_name = " + file_name);

        // WHEN
        threads.push_back(std::async(std::launch::async, thread_func, file_name));
    }

    // THEN
    for (auto& thread : threads)
    {
        thread.get();
        log_entry(log::audit{}, "Thread finished successfully.");
    }
}


INSTANTIATE_TEST_SUITE_P(
    BasicTests, multithreaded_test,
    ::testing::Values(
        std::make_tuple(1, 9, "odt"),
        std::make_tuple(1, 9, "fodt"),
        std::make_tuple(1, 9, "ods"),
        std::make_tuple(1, 9, "fods"),
        std::make_tuple(1, 9, "odp"),
        std::make_tuple(1, 9, "fodp"),
        std::make_tuple(1, 9, "odg"),
        std::make_tuple(1, 9, "fodg"),
        std::make_tuple(1, 9, "rtf"),
        std::make_tuple(1, 9, "doc"),
        std::make_tuple(1, 9, "xls"),
        std::make_tuple(1, 9, "xlsb"),
        std::make_tuple(1, 9, "pdf"),
        std::make_tuple(1, 9, "ppt"),
        std::make_tuple(1, 9, "docx"),
        std::make_tuple(1, 9, "xlsx"),
        std::make_tuple(1, 9, "pptx"),
        std::make_tuple(1, 9, "pages"),
        std::make_tuple(1, 9, "numbers"),
        std::make_tuple(1, 9, "key"),
        std::make_tuple(1, 9, "html"),
        std::make_tuple(1, 1, "pst")
                      ),
    [](const ::testing::TestParamInfo<multithreaded_test::ParamType>& info) {
      std::string name = std::string{ std::get<2>(info.param) } + "_multithreaded_tests";
      return name;
    });


INSTANTIATE_TEST_SUITE_P(
    AllDocumentParsing, document_parsing_tests,
    ::testing::ValuesIn(
        []() {
            auto basic_files = generate_list_for_basic_tests();
            auto misc_files = get_misc_test_files_list();
            std::vector<std::string> all_files;
            all_files.reserve(basic_files.size() + misc_files.size());
            all_files.insert(all_files.end(), basic_files.begin(), basic_files.end());
            all_files.insert(all_files.end(), misc_files.begin(), misc_files.end());
            return all_files;
        }()
    ),
    [](const ::testing::TestParamInfo<document_parsing_tests::ParamType>& info) {
        std::string file_name = info.param;
        escape_test_name(file_name);
        return file_name;
    });

class multi_page_filter_test : public ::testing::TestWithParam<std::tuple<int, int, const char*>>
{
};

TEST_P(multi_page_filter_test, ReadFromPathTests)
{
  const auto [lower, upper, format] = GetParam();
  const int MAX_PAGES = 2;
  std::string prefix = "multi_pages_";
  std::vector<std::string> file_names(upper - lower + 1);
  std::generate(file_names.begin(), file_names.end(), [format_str = format, prefix, i = lower]() mutable
  {
    return prefix + std::to_string(i++) + "." + format_str;
  });

  for(auto& file_name : file_names)
  {
    std::ifstream ifs{ file_name + ".out" };
    ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";

    std::string expected_text{ std::istreambuf_iterator<char>{ifs},
                               std::istreambuf_iterator<char>{}};

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | ocr_parser{} |
        [MAX_PAGES, counter = 0](message_ptr msg, const message_callbacks& emit_message) mutable
        {
            if (msg->is<document::page>())
            {
                ++counter;
                if (counter > MAX_PAGES)
                    return continuation::stop;
            }
            return emit_message(std::move(msg));
        } |
        plain_text_exporter() |
        output_stream;

    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
  }

}

INSTANTIATE_TEST_SUITE_P(
        BasicTests, multi_page_filter_test,
        ::testing::Values(
                std::make_tuple(1, 1, "pdf")
        ),
        [](const ::testing::TestParamInfo<multi_page_filter_test::ParamType>& info) {
          std::string name = std::string{ std::get<2>(info.param) } + "_multi_page_filter_tests";
          return name;
        });
