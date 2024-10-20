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

#include "base64.h"
#include "office_formats_parser_provider.h"
#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include "chaining.h"
#include "detect_by_file_extension.h"
#include "detect_by_signature.h"
#include "error_tags.h"
#include "exception_utils.h"
#include <exception>
#include <future>
#include "fuzzy_match.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <string_view>
#include <tuple>
#include "decompress_archives.h"
#include <fstream>
#include "html_exporter.h"
#include "importer.h"
#include "language.h"
#include <iterator>
#include <array>
#include "magic_enum_iostream.hpp"
#include "mail_parser_provider.h"
#include "meta_data_exporter.h"
#include "../src/standard_filter.h"
#include <optional>
#include <algorithm>
#include "ocr_parser_provider.h"
#include "output.h"
#include "parse_detected_format.h"
#include "plain_text_exporter.h"
#include "plain_text_writer.h"
#include "post.h"
#include <regex>
#include "throw_if.h"
#include "transformer_func.h"
#include "txt_parser.h"
#include "input.h"
#include "log.h"
#include "lru_memory_cache.h"

void escape_test_name(std::string& str)
{
    std::transform(str.cbegin(), str.cend(), str.begin(), [](const auto ch)
        {   if(ch == '.') return '_'; 
            else if(ch == '-') return '_';
            else return ch; 
        }
    );
}

using namespace docwire;

class DocumentTests :public ::testing::TestWithParam<std::tuple<int, int, const char*>> {
protected:
    ParserParameters parameters{};

    void SetUp() override
    {
        parameters += ParserParameters("languages", std::vector { Language::pol });
  }

};

TEST_P(DocumentTests, ParseFromPathTest)
{
    const auto [lower, upper, format] = GetParam();

    for(int i = lower; i <= upper; ++i)
    {
        // GIVEN
        std::string file_name{ std::to_string(i) + "." + format };

        std::ifstream ifs{ file_name + ".out" };
        ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";
        
        std::string expected_text{ std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{}};

        SCOPED_TRACE("file_name = " + file_name);

        // WHEN
        std::ostringstream output_stream{};

        std::filesystem::path(file_name) |
          ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>(parameters) |
          PlainTextExporter() |
          output_stream;

        // THEN
        EXPECT_EQ(expected_text, output_stream.str());
    }
}

TEST_P(DocumentTests, ParseFromStreamTest)
{
    const auto [lower, upper, format] = GetParam();

    for(int i = lower; i <= upper; ++i)
    {
        // GIVEN
        std::string file_name{ std::to_string(i) + "." + format };

        std::ifstream ifs{ file_name + ".out" };
        ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";
        
        std::string expected_text{ std::istreambuf_iterator<char>{ifs},
            std::istreambuf_iterator<char>{}};

        SCOPED_TRACE("file_name = " + file_name);

        // WHEN
        std::ostringstream output_stream{};

        std::ifstream { file_name, std::ios_base::binary } |
          ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>(parameters) |
          PlainTextExporter() |
          output_stream;

        // THEN
        EXPECT_EQ(expected_text, output_stream.str());
    }
}

INSTANTIATE_TEST_SUITE_P(
    BasicTests, DocumentTests,
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
        std::make_tuple(1, 6, "bmp"),
        std::make_tuple(1, 6, "jpg"),
        std::make_tuple(1, 6, "jpeg"),
        std::make_tuple(1, 6, "png"),
        std::make_tuple(1, 6, "tiff"),
        std::make_tuple(1, 6, "webp"),
        std::make_tuple(1, 1, "pst")
                      ),
    [](const ::testing::TestParamInfo<DocumentTests::ParamType>& info) {
      std::string name = std::string{ std::get<2>(info.param) } + "_basic_tests";
      return name;
    });

class MetadataTest : public ::testing::TestWithParam<const char*>
{
protected:

    static constexpr std::array<std::string_view, 2> names
    {
        "meta_libreoffice_3.5_created", 
        "meta_libreoffice_3.5_modified" 
    };
};

TEST_P(MetadataTest, ParseFromPathTest)
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
          ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
          MetaDataExporter() |
          output_stream;

        // THEN
        EXPECT_EQ(expected_text, output_stream.str());
    }
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFileMetadataTests, MetadataTest,
    ::testing::Values(
        "odt", "ods", "odp", "odg", "rtf", "doc", "xls", "ppt", "docx", "xlsx", "pptx", "html"
                      ),
    [](const ::testing::TestParamInfo<MetadataTest::ParamType>& info) {
      std::string name = std::string{ info.param } + "_basic_metadata_tests";
      return name;
    });

class CallbackTest : public ::testing::TestWithParam<std::tuple<const char*, const char*, NewNodeCallback>>
{
protected:
    ParserParameters parameters{};

    void SetUp() override
    {
  }
};


TEST_P(CallbackTest, ParseFromPathTest)
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
        ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
        TransformerFunc(callback) |
        PlainTextExporter() |
        output_stream;

    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
}

INSTANTIATE_TEST_SUITE_P(
    StandardFilterTests, CallbackTest,
    ::testing::Values(
        std::make_tuple("1.pst", "1.pst.2.out", StandardFilter::filterByMailMinCreationTime(1644216799))
                      ));

class HTMLWriterTest : public ::testing::TestWithParam<const char*>
{
};

TEST_P(HTMLWriterTest, ParseFromPathTest)
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
        ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
        HtmlExporter() |
        output_stream;
        
    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFileHTMLWriterTest, HTMLWriterTest,
    ::testing::Values(
        "1.docx", "2.docx", "3.docx", "4.docx", "5.docx", "6.docx", "7.docx", "8.docx", "9.docx", "10.docx",
        "1.doc", "2.doc", "3.doc", "4.doc", "5.doc", "6.doc", "7.doc", "8.doc", "9.doc",
        "1.html", "2.html", "3.html", "4.html", "5.html", "6.html", "7.html", "8.html", "9.html",
        "first.eml"
                      ),
    [](const ::testing::TestParamInfo<HTMLWriterTest::ParamType>& info) {
        std::string file_name = info.param;
        escape_test_name(file_name);

        std::string name = file_name + "_basic_html_tests";
        return name;
    });

class MiscDocumentTest : public ::testing::TestWithParam<const char*>
{
};

TEST_P(MiscDocumentTest, ParseFromPathTest)
{
    // GIVEN
    auto name = GetParam();
    std::string file_name{ name };

    std::ifstream ifs{ file_name + ".out" };
    ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";
    
    std::string expected_text{ std::istreambuf_iterator<char>{ifs},
        std::istreambuf_iterator<char>{}};

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    ParserParameters parameters{};
	if (file_name.find(".png") != std::string::npos)
	{
		std::vector<std::string> fn_parts;
		boost::split(fn_parts, file_name, boost::is_any_of("-."));
		std::vector<Language> langs;
		for (std::string fn_part: fn_parts)
		{
			std::string_view fn_part_view = fn_part;
			std::optional<Language> lang = magic_enum::enum_cast<Language>(fn_part_view, magic_enum::case_insensitive);
			if (lang)
				langs.push_back(*lang);
		}
		parameters += ParserParameters("languages", langs);
	}

    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
        DecompressArchives() |
        ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>(parameters) |
        PlainTextExporter() |
        output_stream;
        
    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
}

INSTANTIATE_TEST_SUITE_P(
    MiscellaneousTest, MiscDocumentTest,
    ::testing::Values(
        "biff5.xls",
        "hebrew_1.rtf",
        "hebrew_2.rtf",
        "japanese.xls",
        "embedded_spreadsheet.doc",
        "encoding_in_table.doc",
        "fields.doc",
        "fields.docx",
        "crash_test.xls",
        "special_para_cmds.rtf",
        "header_footer.doc",
        "shared_strings.xls",
        "shared_strings.xlsb",
        "whitespaces.odt",
        "whitespaces.doc",
        "whitespaces.docx",
        "space_attributes_in_non_standard_places.docx",
        "ole.odt",
        "ole.fodt",
        "nested_tables.html",
        "first.eml",
        "second.eml",
        "third.eml",
        "fourth.eml",
        "html_with_doc_ext.doc",
        "html_with_xls_ext.xls",
        "rtf_with_doc_ext.doc",
        "comments_libreoffice_3.5.odt",
        "comments_libreoffice_3.5.doc",
        "comments_libreoffice_3.5.docx",
        "comments_libreoffice_3.5.rtf",
        "gb18030_without_bom.text",
        "utf16_with_bom.text",
        "test.asp",
        "test.aspx",
        "test.css",
        "test.fpp",
        "test.js",
        "test.json",
        "test.txt",
        "test.xml",
        "empty_cells.xlsx",
        "test.zip",
        "test.tar",
        "test.tar.gz",
        "test.tar.bz2",
        "test.tar.xz",
        "test.rar",
		"multilang-chi_sim-fra-deu-eng.png",
		"multilang-chi_tra-rus-jpn.png",
		"multilang-spa-lat-grc.png",
		"multilang-hin-san-swa-kor-eng.png"
                      ),
    [](const ::testing::TestParamInfo<MiscDocumentTest::ParamType>& info) {
        std::string file_name = info.param;
        escape_test_name(file_name);
        std::string name = file_name + "_read_from_file_test";
        return name;
    });

class PasswordProtectedTest : public ::testing::TestWithParam<const char*>
{
};

TEST_P(PasswordProtectedTest, MajorTestingModule)
{
    // GIVEN
    auto format = GetParam();
    std::string file_name{ std::string{"password_protected."} + format };

    std::ifstream ifs{ file_name + ".out" };
    ASSERT_TRUE(ifs.good()) <<  "File " << file_name << ".out" << " not found\n";
    
    std::string expected_text{ std::istreambuf_iterator<char>{ifs},
        std::istreambuf_iterator<char>{}};

    SCOPED_TRACE("file_name = " + file_name);

    // WHEN
    std::ostringstream output_stream{};

    try 
    {
        std::filesystem::path{file_name} |
            ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
            PlainTextExporter() |
            output_stream;
        FAIL() << "We are not supporting password protected files yet. Why didn\'t we catch exception?\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << errors::diagnostic_message(ex);
        ASSERT_TRUE(errors::contains_type<errors::file_is_encrypted>(ex));
    }   
}

INSTANTIATE_TEST_SUITE_P(
    ReadFromFilePasswordProtectedTests, PasswordProtectedTest,
    ::testing::Values(
        "doc", "docx", "key", "pages", "numbers", "odp", "pdf", "ppt", "pptx", "xls", "xlsb", "xlsx"
                      ),
    [](const ::testing::TestParamInfo<PasswordProtectedTest::ParamType>& info) {
        std::string format = info.param;
        std::string name = format + "_password_protected_test";
        return name;
    });

class MultithreadedTest : public ::testing::TestWithParam<std::tuple<int, int, const char*>>
{
};

void thread_func(const std::string& file_name)
{
    std::ostringstream output_stream{};

    std::filesystem::path{file_name} |
      ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
      PlainTextExporter() |
      output_stream;

    std::filesystem::path{file_name} |
      ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
      MetaDataExporter() |
      output_stream;
}

TEST_P(MultithreadedTest, ReadFromFileTests)
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
        docwire_log(info) << "Thread finished successfully.";
    }
}


INSTANTIATE_TEST_SUITE_P(
    BasicTests, MultithreadedTest,
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
        std::make_tuple(1, 6, "bmp"),
        std::make_tuple(1, 6, "jpg"),
        std::make_tuple(1, 6, "jpeg"),
        std::make_tuple(1, 6, "png"),
        std::make_tuple(1, 6, "tiff"),
        std::make_tuple(1, 6, "webp"),
        std::make_tuple(1, 1, "pst")
                      ),
    [](const ::testing::TestParamInfo<MultithreadedTest::ParamType>& info) {
      std::string name = std::string{ std::get<2>(info.param) } + "_multithreaded_tests";
      return name;
    });

class MultiPageFilterTest : public ::testing::TestWithParam<std::tuple<int, int, const char*>>
{
};

TEST_P(MultiPageFilterTest, ReadFromPathTests)
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
        ParseDetectedFormat<OfficeFormatsParserProvider, MailParserProvider, OcrParserProvider>() |
        TransformerFunc([MAX_PAGES, counter = 0](Info &info) mutable
        {
            if (std::holds_alternative<tag::Page>(info.tag))
            {
                ++counter;
                if (counter > MAX_PAGES)
                    info.cancel = true;
            }
        }) |
        PlainTextExporter() |
        output_stream;

    // THEN
    EXPECT_EQ(expected_text, output_stream.str());
  }

}

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

INSTANTIATE_TEST_SUITE_P(
        BasicTests, MultiPageFilterTest,
        ::testing::Values(
                std::make_tuple(1, 1, "pdf")
        ),
        [](const ::testing::TestParamInfo<MultithreadedTest::ParamType>& info) {
          std::string name = std::string{ std::get<2>(info.param) } + "_multi_page_filter_tests";
          return name;
        });

TEST(Http, Post)
{
    std::ostringstream output_stream{};
	ASSERT_NO_THROW(
	{
		std::ifstream("1.docx", std::ios_base::binary)
			| ParseDetectedFormat<OfficeFormatsParserProvider>()
			| PlainTextExporter()
			| http::Post("https://postman-echo.com/post")
			| output_stream;
	});

	using namespace boost::json;
	value output_val = parse(output_stream.str());
    ASSERT_TRUE(output_val.is_object());
    ASSERT_TRUE(output_val.as_object()["headers"].is_object());
    ASSERT_STREQ(output_val.as_object()["headers"].as_object()["content-type"].as_string().c_str(), "application/json");
    ASSERT_STREQ(output_val.as_object()["headers"].as_object()["content-length"].as_string().c_str(), "46");
    ASSERT_STREQ(output_val.as_object()["data"].as_string().c_str(), "<http://www.silvercoders.com/>hyperlink test\n\n");
}

TEST(Http, PostForm)
{
    std::ostringstream output_stream{};
	ASSERT_NO_THROW(
	{
		std::ifstream("1.docx", std::ios_base::binary)
			| ParseDetectedFormat<OfficeFormatsParserProvider>()
			| PlainTextExporter()
			| http::Post("https://postman-echo.com/post", {{"field1", "value1"}, {"field2", "value2"}}, "file", DefaultFileName("file.docx"))
			| output_stream;
	});

    using namespace boost::json;
    value output_val = parse(output_stream.str());
    ASSERT_TRUE(output_val.is_object());
    ASSERT_TRUE(output_val.as_object()["headers"].is_object());
    ASSERT_THAT(std::string{output_val.as_object()["headers"].as_object()["content-type"].as_string()},
                ::testing::StartsWith("multipart/form-data; boundary="));
    ASSERT_STREQ(output_val.as_object()["headers"].as_object()["content-length"].as_string().c_str(), "458");
    ASSERT_TRUE(output_val.as_object()["form"].is_object());
    ASSERT_STREQ(output_val.as_object()["form"].as_object()["field1"].as_string().c_str(), "value1");
    ASSERT_STREQ(output_val.as_object()["form"].as_object()["field2"].as_string().c_str(), "value2");
    ASSERT_TRUE(output_val.as_object()["files"].is_object());
    ASSERT_STREQ(output_val.as_object()["files"].as_object()["file.txt"].as_string().c_str(), "data:application/octet-stream;base64,PGh0dHA6Ly93d3cuc2lsdmVyY29kZXJzLmNvbS8+aHlwZXJsaW5rIHRlc3QKCg==");
}

TEST (errors, throwing)
{
    ASSERT_NO_THROW(throw_if(2 > 3, "test"));
    try
    {
        throw make_error("test");
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(const char*));
        ASSERT_EQ(e.context_string(), "test");
    }
    try
    {
        std::string s { "test" };
        throw make_error(s);
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(std::pair<std::string, std::string>));
        ASSERT_EQ(e.context_string(), "s: test");
    }
    try
    {
        throw make_error(errors::network_error{});
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(errors::network_error));
        ASSERT_EQ(e.context_string(), "network error");
    }
    try
    {
        throw_if("2 < 3", errors::file_is_encrypted{}, errors::backtrace_entry{});
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(errors::backtrace_entry));
        ASSERT_EQ(e.context_string(), "backtrace entry");
        try
        {
            std::rethrow_if_nested(e);
            FAIL() << "Expected nested exception";
        }
        catch (const errors::base& e)
        {
            ASSERT_EQ(e.context_type(), typeid(errors::file_is_encrypted));
            ASSERT_EQ(e.context_string(), "file is encrypted");
            try
            {
                std::rethrow_if_nested(e);
                FAIL() << "Expected nested exception";
            }
            catch (const errors::base& e)
            {
                ASSERT_EQ(e.context_type(), typeid(std::pair<std::string, const char*>));
                ASSERT_EQ(e.context_string(), "triggering_condition: \"2 < 3\"");
            }
        }
    }
}

TEST(errors, diagnostic_message)
{
    std::string message;
    errors::source_location err2_loc, err3_loc;
    try
    {
        try
        {
            try
            {
                throw std::runtime_error{"level 1 exception"};
            }
            catch (const std::exception& e)
            {
                err2_loc = current_location();
                std::throw_with_nested(make_error("level 2 exception"));
            }
        }
        catch (const std::exception& e)
        {
            err3_loc = current_location();
            std::throw_with_nested(make_error("level 3 exception"));
        }
    }
    catch (const std::exception& e)
    {
        message = errors::diagnostic_message(e);
    }
	ASSERT_EQ(message,
        std::string{"Error \"level 1 exception\"\n"} +
        "No location information available\n"
        "with context \"level 2 exception\"\n"
        "in " + err2_loc.function_name() + "\n"
        "at " + err2_loc.file_name() + ":" + std::to_string(err2_loc.line() + 1) + "\n"
        "with context \"level 3 exception\"\n"
        "in " + err3_loc.function_name() + "\n"
        "at " + + err3_loc.file_name() + ":" + std::to_string(err3_loc.line() + 1) + "\n"
    );
}

std::string sanitize_log_text(const std::string& orig_log_text)
{
    using namespace boost::json;
	std::string log_text = "[\n";
	value log_val = parse(orig_log_text + "]");
	for (int i = 0; i < log_val.as_array().size(); i++)
	{
		if (i > 0)
			log_text += ",\n";
		log_val.as_array()[i].as_object()["timestamp"] = "<timestamp>";
		log_val.as_array()[i].as_object()["thread_id"] = "<thread_id>";
		log_val.as_array()[i].as_object()["line"] = "<line>";
		log_text += serialize(log_val.as_array()[i]);
	}
	log_text += "\n]\n";
    return log_text;
}

TEST(Logging, Dereferenceable)
{
	static_assert(is_iterable<std::vector<int>>::value);
	static_assert(is_iterable<std::list<int>>::value);
	static_assert(!is_iterable<std::optional<int>>::value);
	static_assert(!is_iterable<std::unique_ptr<int>>::value);

	static_assert(!is_dereferenceable<std::vector<int>>::value);
	static_assert(!is_dereferenceable<std::list<int>>::value);
	static_assert(is_dereferenceable<std::optional<int>>::value);
	static_assert(is_dereferenceable<std::unique_ptr<int>>::value);

	std::stringstream log_stream;
	set_log_stream(&log_stream);
	set_log_verbosity(debug);

	docwire_log(debug) << std::optional<int>(1);
	docwire_log(debug) << std::optional<int>();
	docwire_log(debug) << std::make_unique<int>(1);
	docwire_log(debug) << std::unique_ptr<int>();
	docwire_log(debug) << std::make_shared<int>(1);
	docwire_log(debug) << std::shared_ptr<int>();

	set_log_verbosity(info);
	set_log_stream(&std::clog);

    std::string log_text = sanitize_log_text(log_stream.str());
	ASSERT_EQ(read_test_file("logging_dereferenceable.out.json"), log_text);
}

TEST(Logging, CerrLogRedirection)
{
	std::stringstream log_stream;
	set_log_stream(&log_stream);
	set_log_verbosity(debug);

    cerr_log_redirection cerr_redirection(docwire_current_source_location());
	std::cerr << "Cerr test log message line 1" << std::endl;
    std::cerr << "Cerr test log message line 2" << std::endl;
	cerr_redirection.restore();

    set_log_verbosity(info);
	set_log_stream(&std::clog);

    std::string log_text = sanitize_log_text(log_stream.str());
    ASSERT_EQ(read_test_file("logging_cerr_log_redirection.out.json"), log_text);
}

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

namespace
{

std::string create_datasource_test_data_str()
{
    std::vector<std::byte> test_data;
    test_data.reserve(100 * 256);
    for (int i = 0; i < 100; i++)
        for (int b = 0; b < 256; b++)
            test_data.push_back(std::byte{static_cast<unsigned char>(b)});
    std::string test_data_str = std::string(reinterpret_cast<char const*>(&test_data[0]), test_data.size()) + "test";
    return test_data_str;
}

} // anonymous namespace

TEST(DataSource, verify_input_data)
{
    std::string test_data_str = create_datasource_test_data_str();
    ASSERT_EQ(test_data_str.size(), 100 * 256 + 4);
    ASSERT_EQ(test_data_str[0], static_cast<char>(std::byte{0}));
    ASSERT_EQ(test_data_str[255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(test_data_str[99 * 256], static_cast<char>(std::byte{0}));
    ASSERT_EQ(test_data_str[99 * 256 + 255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(test_data_str[100 * 256], 't');
}

TEST(DataSource, vector_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    std::vector<std::byte> vector{reinterpret_cast<const std::byte*>(test_data_str.data()), reinterpret_cast<const std::byte*>(test_data_str.data()) + test_data_str.size()};
    data_source data{vector, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, vector_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{std::vector<std::byte>{reinterpret_cast<const std::byte*>(test_data_str.data()), reinterpret_cast<const std::byte*>(test_data_str.data()) + test_data_str.size()}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, string_view_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    std::string_view string_view{test_data_str};
    data_source data{string_view, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, string_view_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{std::string_view{test_data_str}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, seekable_stream_ptr_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    seekable_stream_ptr stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, seekable_stream_ptr_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{seekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, unseekable_stream_ptr_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    unseekable_stream_ptr stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, unseekable_stream_ptr_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{unseekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(PlainTextExporter, table_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><table><tr><td>table inside table without cells</td></tr></table></table></html>"} |
            ParseDetectedFormat<parser_provider<HTMLParser>>{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, table_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr><table><tr><td>table inside table without cells</td></tr></table></tr></table></html>"} |
            ParseDetectedFormat<parser_provider<HTMLParser>>{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, cell_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><thead><td>cell without row</td></thead></table></html>"} |
            ParseDetectedFormat<parser_provider<HTMLParser>>{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, content_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table>content without rows</table></html>"} |
            ParseDetectedFormat<parser_provider<HTMLParser>>{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, content_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr>content without cell</tr></table></html>"} |
            ParseDetectedFormat<parser_provider<HTMLParser>>{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, eol_sequence_crlf)
{
    auto exporter = std::make_shared<PlainTextExporter>(eol_sequence{"\r\n"});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    std::vector<Tag> tags
    {
        tag::Document{},
        tag::Text{.text = "Line1"},
        tag::BreakLine{},
        tag::Text{.text = "Line2"},
        tag::CloseDocument{}
    };
    for (auto tag: tags)
    {
        Info info{tag};
        exporter->process(info);    
    }
    ASSERT_EQ(output_stream.str(), "Line1\r\nLine2\r\n");
}

TEST(PlainTextExporter, custom_link_formatting)
{
    auto exporter = std::make_shared<PlainTextExporter>(
        eol_sequence{"\n"},
        link_formatter{
            .format_opening = [](const tag::Link& link){ return (link.url ? "(" + *link.url + ")" : "") + "["; },
            .format_closing = [](const tag::CloseLink& link){ return "]"; }
        });
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    std::vector<Tag> tags
    {
        tag::Document{},
        tag::Link{.url = "https://docwire.io"},
        tag::Text{.text = "DocWire SDK home page"},
        tag::CloseLink{},
        tag::CloseDocument{}
    };
    for (auto tag: tags)
    {
        Info info{tag};
        exporter->process(info);    
    }
    ASSERT_EQ(output_stream.str(), "(https://docwire.io)[DocWire SDK home page]\n");
}

TEST(Input, data_source_with_file_ext)
{
    std::ostringstream output_stream{};
    data_source{seekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios::binary)}, file_extension{".doc"}} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, path_ref)
{
    std::ostringstream output_stream{};
    std::filesystem::path path{"1.doc"};
    path | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, path_temp)
{
    std::ostringstream output_stream{};    
    std::filesystem::path{"1.doc"} | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, vector_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::vector<std::byte> vector{reinterpret_cast<const std::byte*>(str.data()), reinterpret_cast<const std::byte*>(str.data()) + str.size()};
    vector | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, vector_temp)
{
    std::ostringstream output_stream{};    
    std::string str = read_binary_file("1.doc");
    std::vector<std::byte>{reinterpret_cast<const std::byte*>(str.data()), reinterpret_cast<const std::byte*>(str.data()) + str.size()} |
        ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, span_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::span<const std::byte> span{reinterpret_cast<const std::byte*>(str.data()), str.size()};
    span | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, span_temp)
{
    std::ostringstream output_stream{};    
    std::string str = read_binary_file("1.doc");
    std::span<const std::byte>{reinterpret_cast<const std::byte*>(str.data()), str.size()} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    str | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_temp)
{
    std::ostringstream output_stream{};    
    read_binary_file("1.doc") | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_view_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_binary_file("1.doc");
    std::string_view string_view{str};
    string_view | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_view_temp)
{
    std::ostringstream output_stream{};    
    std::string_view{read_binary_file("1.doc")} | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_shared_ptr)
{
    std::ostringstream output_stream{};
    std::make_shared<std::ifstream>("1.doc", std::ios_base::binary) | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_temp)
{
    std::ostringstream output_stream{};
    std::ifstream{"1.doc", std::ios_base::binary} | ParseDetectedFormat<parser_provider<DOCParser>>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(fuzzy_match, ratio)
{
    ASSERT_EQ(docwire::fuzzy_match::ratio("hello", "hello"), 100.0);
    ASSERT_EQ(docwire::fuzzy_match::ratio("hello", "helll"), 80.0);
}

namespace docwire::tag
{

void PrintTo(const Text& text, std::ostream* os)
{
    *os << testing::PrintToString(text.text);
}

}

TEST(TXTParser, lines)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    std::string test_input {"Line ends with LF\nLine ends with CR\rLine ends with CRLF\r\nLine without EOL"};
    docwire::data_source{test_input, docwire::file_extension{".txt"}} |
        TXTParser{} | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with LF"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with CR"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with CRLF"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line without EOL"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
    tags.clear();
    docwire::ParserParameters parameters{"TXTParser::parse_lines", false};
    docwire::data_source{test_input, docwire::file_extension{".txt"}} |
        TXTParser{}.withParameters(parameters) | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with LF"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("\n"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with CR"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("\r"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line ends with CRLF"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("\r\n"))),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line without EOL"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
    tags.clear();
    parameters += docwire::ParserParameters{"TXTParser::parse_paragraphs", false};
    docwire::data_source{test_input, docwire::file_extension{".txt"}} |
        TXTParser{}.withParameters(parameters) | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq(test_input))),
        VariantWith<tag::CloseDocument>(_)
    ));
}

TEST(TXTParser, paragraphs)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    docwire::data_source{
            std::string{"Paragraph 1 Line 1\nParagraph 1 Line 2\n\nParagraph 2 Line 1"},
            docwire::file_extension{".txt"}} |
        TXTParser{} | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph 1 Line 1"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph 1 Line 2"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph 2 Line 1"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
    tags.clear();
    docwire::data_source{std::string{"\nLine\n"}, docwire::file_extension{".txt"}} |
        TXTParser{} | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
    tags.clear();
    docwire::data_source{std::string{"\nLine\n"}, docwire::file_extension{".txt"}} |
        TXTParser{}.withParameters(
            docwire::ParserParameters{"TXTParser::parse_paragraphs", false}) |
        tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::CloseDocument>(_)
    ));    
}

TEST(base64, encode)
{
    const std::string input_str { "test" };
    const std::span<const std::byte> input_data { reinterpret_cast<const std::byte*>(input_str.c_str()), input_str.size() };
    std::string encoded = base64::encode(input_data);
    ASSERT_EQ(encoded, "dGVzdA==");
}

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

TEST(detect, by_file_extension)
{
    data_source data { std::filesystem::path{"1.docx"} };
    try {
        detect::by_file_extension(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    ASSERT_TRUE(data.content_type.has_value());
    mime_type content_type = *data.content_type;
    ASSERT_EQ(content_type.v, "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
}

TEST(detect, by_signature)
{
    data_source data { seekable_stream_ptr { std::make_shared<std::ifstream>("1.doc", std::ios_base::binary) }};
    try {
        detect::by_signature(data);
    }
    catch (const std::exception& e) {
        FAIL() << errors::diagnostic_message(e);
    }
    mime_type content_type = *data.content_type;
    ASSERT_EQ(content_type.v, "application/msword");
}
