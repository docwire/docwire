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
#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include "chaining.h"
#include "content_type.h"
#include "content_type_by_file_extension.h"
#include "content_type_by_signature.h"
#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"
#include "data_source.h"
#include "error_hash.h" // IWYU pragma: keep
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
#include "language.h"
#include <iterator>
#include <array>
#include <magic_enum/magic_enum_iostream.hpp>
#include "mail_parser.h"
#include "meta_data_exporter.h"
#include "../src/standard_filter.h"
#include <optional>
#include <algorithm>
#include "ocr_parser.h"
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
#include "tags.h"
#include "throw_if.h"
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
    const std::vector<Language> languages { Language::pol };
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

        try
        {
            std::filesystem::path(file_name) |
                content_type::by_file_extension::detector{} |
                office_formats_parser{} | mail_parser{} | OCRParser{languages} |
                PlainTextExporter() |
                output_stream;
        }
        catch (const std::exception& e)
        {
            FAIL() << errors::diagnostic_message(e);
        }

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

        try
        {
            std::ifstream { file_name, std::ios_base::binary } |
            content_type::detector{} |
            office_formats_parser{} | mail_parser{} | OCRParser{languages} |
            PlainTextExporter() |
            output_stream;
        }
        catch (const std::exception& e)
        {
            FAIL() << errors::diagnostic_message(e);
        }

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
            content_type::by_file_extension::detector{} |
          office_formats_parser{} | mail_parser{} | OCRParser{} |
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
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | OCRParser{} |
        callback |
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
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | OCRParser{} |
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
    std::vector<Language> langs;
	if (file_name.find(".png") != std::string::npos)
	{
		std::vector<std::string> fn_parts;
		boost::split(fn_parts, file_name, boost::is_any_of("-."));
		for (std::string fn_part: fn_parts)
		{
			std::string_view fn_part_view = fn_part;
			std::optional<Language> lang = magic_enum::enum_cast<Language>(fn_part_view, magic_enum::case_insensitive);
			if (lang)
				langs.push_back(*lang);
		}
	}

    std::ostringstream output_stream{};

    try
    {
        std::filesystem::path{file_name} |
            DecompressArchives() |
            content_type::detector{} |
            office_formats_parser{} | mail_parser{} | OCRParser{langs} |
            PlainTextExporter() |
            output_stream;
    }
    catch (const std::exception& e)
    {
        FAIL() << errors::diagnostic_message(e);
    }
        
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
            content_type::by_file_extension::detector{} |
            office_formats_parser{} | mail_parser{} | OCRParser{} |
            PlainTextExporter() |
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
      content_type::by_file_extension::detector{} |
      office_formats_parser{} | mail_parser{} | OCRParser{} |
      PlainTextExporter() |
      output_stream;

    std::filesystem::path{file_name} |
      content_type::by_file_extension::detector{} |
      office_formats_parser{} | mail_parser{} | OCRParser{} |
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
        content_type::by_file_extension::detector{} |
        office_formats_parser{} | mail_parser{} | OCRParser{} |
        [MAX_PAGES, counter = 0](Info &info) mutable
        {
            if (std::holds_alternative<tag::Page>(info.tag))
            {
                ++counter;
                if (counter > MAX_PAGES)
                    info.cancel = true;
            }
        } |
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
            | content_type::by_signature::detector{}
            | office_formats_parser{}
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
            | content_type::by_signature::detector{}
            | office_formats_parser{}
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
        throw make_error(errors::network_failure{});
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(errors::network_failure));
        ASSERT_EQ(e.context_string(), "network failure error tag");
    }
    try
    {
        std::string s { "test" };
        throw_if(2 < 3, errors::file_encrypted{}, s);
    }
    catch (const errors::base& e)
    {
        ASSERT_EQ(e.context_type(), typeid(std::pair<std::string, std::string>));
        ASSERT_EQ(e.context_string(), "s: test");
        try
        {
            std::rethrow_if_nested(e);
            FAIL() << "Expected nested exception";
        }
        catch (const errors::base& e)
        {
            ASSERT_EQ(e.context_type(), typeid(errors::file_encrypted));
            ASSERT_EQ(e.context_string(), "file encrypted error tag");
            try
            {
                std::rethrow_if_nested(e);
                FAIL() << "Expected nested exception";
            }
            catch (const errors::base& e)
            {
                ASSERT_EQ(e.context_type(), typeid(std::pair<std::string, const char*>));
                ASSERT_EQ(e.context_string(), "triggering_condition: 2 < 3");
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

template <typename Inner>
void test_make_nested(const Inner& inner)
{
    auto e = errors::make_nested(inner, std::logic_error{"level 2"}, std::runtime_error{"level 3"});
    static_assert(std::is_same_v<std::decay_t<decltype(e)>, errors::nested<std::runtime_error>>);
    ASSERT_STREQ(e.what(), "level 3");
    try
    {
        std::rethrow_if_nested(e);
        FAIL() << "Expected nested exception";
    }
    catch(const std::exception& e)
    {
        ASSERT_EQ(typeid(e), typeid(errors::nested<std::logic_error>));
        ASSERT_STREQ(e.what(), "level 2");
        try
        {
            std::rethrow_if_nested(e);
            FAIL() << "Expected nested exception";
        }
        catch(const std::exception& e)
        {
            ASSERT_EQ(typeid(e), typeid(std::out_of_range));
            ASSERT_STREQ(e.what(), "level 1");
        }
        catch(...)
        {
            FAIL() << "Unexpected exception";
        }
    }
    catch(...)
    {
        FAIL() << "Unexpected exception";
    }
}

TEST(errors, make_nested)
{
    test_make_nested(std::out_of_range{"level 1"});
}

TEST(errors, make_nested_current_exception)
{
    try
    {
        throw std::out_of_range{"level 1"};
    }
    catch (std::exception&)
    {
        test_make_nested(std::current_exception());
    }
    catch(...)
    {
        FAIL() << "Unexpected exception";
    }
}

TEST(errors, hashing)
{
    std::hash<errors::base> hasher;
    auto e1 = make_error("test");
    auto e2 = make_error("test");
    auto e3 = make_error("test");
    auto n1 = make_nested(e1, e3);
    auto n2 = make_nested(e2, e3);
    ASSERT_NE(hasher(e1), hasher(e2));
    ASSERT_NE(hasher(n1), hasher(n2));
    ASSERT_NE(hasher(n1), hasher(e1));
    ASSERT_NE(hasher(n2), hasher(e2));
    ASSERT_NE(hasher(n1), hasher(e3));
    ASSERT_NE(hasher(n2), hasher(e3));
    ASSERT_EQ(hasher(e1), hasher(e1));
    ASSERT_EQ(hasher(e2), hasher(e2));
    ASSERT_EQ(hasher(e3), hasher(e3));
    ASSERT_EQ(hasher(n1), hasher(n1));
    ASSERT_EQ(hasher(n2), hasher(n2));
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

template <typename stream_ptr_type>
void test_data_source_incremental()
{
    std::string test_data_str = create_datasource_test_data_str();
    stream_ptr_type stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr};
    for (unsigned int i = 1; i <= 9; i++)
    {
        ASSERT_EQ(data.string(length_limit{i * 256}), test_data_str.substr(0, i * 256));
        ASSERT_EQ(stream_ptr.v->tellg(), i * 256);
    }
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, incremental_unseekable_stream_ptr)
{
    test_data_source_incremental<unseekable_stream_ptr>();
}

TEST(DataSource, increamental_seekable_stream_ptr)
{
    test_data_source_incremental<seekable_stream_ptr>();
}

template<typename Exporter>
void test_table_exporting(const std::string& expected)
{
    std::ostringstream output_stream{};
    auto parsing_chain = Exporter{} | output_stream;
    std::vector<Tag> tags
    {
        tag::Document{},
        tag::Table{},
        tag::Caption{},
        tag::Text{.text = "Table caption"},
        tag::CloseCaption{},
        tag::TableRow{},
        tag::TableCell{},
        tag::Text{.text = "Header 1"},
        tag::CloseTableCell{},
        tag::TableCell{},
        tag::Text{.text = "Header 2"},
        tag::CloseTableCell{},
        tag::CloseTableRow{},
        tag::TableRow{},
        tag::TableCell{},
        tag::Text{.text = "Row 1 Cell 1"},
        tag::CloseTableCell{},
        tag::TableCell{},
        tag::Text{.text = "Row 1 Cell 2"},
        tag::CloseTableCell{},
        tag::CloseTableRow{},
        tag::TableRow{},
        tag::TableCell{},
        tag::Text{.text = "Row 2 Cell 1"},
        tag::CloseTableCell{},
        tag::TableCell{},
        tag::Text{.text = "Row 2 Cell 2"},
        tag::CloseTableCell{},
        tag::CloseTableRow{},
        tag::TableRow{},
        tag::TableCell{},
        tag::Text{.text = "Footer 1"},
        tag::CloseTableCell{},
        tag::TableCell{},
        tag::Text{.text = "Footer 2"},
        tag::CloseTableCell{},
        tag::CloseTableRow{},
        tag::CloseTable{},
        tag::CloseDocument{}
    };
    for (auto tag: tags)
    {
        parsing_chain(tag);
    }
    ASSERT_EQ(output_stream.str(), expected);
}

TEST(HtmlExporter, table)
{
    test_table_exporting<HtmlExporter>(
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"utf-8\">\n"
        "<title>DocWire</title>\n"
        "</head>\n"
        "<body>\n"
        "<table>"
        "<caption>Table caption</caption>"
        "<tr><td>Header 1</td><td>Header 2</td></tr>"
        "<tr><td>Row 1 Cell 1</td><td>Row 1 Cell 2</td></tr>"
        "<tr><td>Row 2 Cell 1</td><td>Row 2 Cell 2</td></tr>"
        "<tr><td>Footer 1</td><td>Footer 2</td></tr>"
        "</table>"
        "</body>\n"
        "</html>\n");
}

TEST(PlainTextExporter, table)
{
    test_table_exporting<PlainTextExporter>(
        "Table caption\n"
        "Header 1      Header 2    \n"
        "Row 1 Cell 1  Row 1 Cell 2\n"
        "Row 2 Cell 1  Row 2 Cell 2\n"
        "Footer 1      Footer 2    \n"
        "\n");
}

TEST(PlainTextExporter, table_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><table><tr><td>table inside table without cells</td></tr></table></table></html>"} |
            HTMLParser{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, table_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr><table><tr><td>table inside table without cells</td></tr></table></tr></table></html>"} |
            HTMLParser{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, cell_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><thead><td>cell without row</td></thead></table></html>"} |
            HTMLParser{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, content_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table>content without rows</table></html>"} |
            HTMLParser{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, content_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr>content without cell</tr></table></html>"} |
            HTMLParser{} |
            PlainTextExporter{} |
            std::ostringstream{}
    );
}

TEST(PlainTextExporter, eol_sequence_crlf)
{
    PlainTextExporter exporter{eol_sequence{"\r\n"}};
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
        parsing_chain(tag);
    }
    ASSERT_EQ(output_stream.str(), "Line1\r\nLine2\r\n");
}

TEST(PlainTextExporter, custom_link_formatting)
{
    PlainTextExporter exporter(
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
        parsing_chain(tag);
    }
    ASSERT_EQ(output_stream.str(), "(https://docwire.io)[DocWire SDK home page]\n");
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
    ASSERT_EQ(owned.get().value(), expected_result);
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
    std::string_view{read_binary_file("1.doc")} | content_type::by_signature::detector{} |
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

namespace docwire
{

void PrintTo(const mime_type& mt, std::ostream* os)
{
    *os << stringify(mt);
}

void PrintTo(const confidence& c, std::ostream* os)
{
    *os << stringify(c);
}

} // namespace docwire

TEST(TXTParser, lines)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    std::string test_input {"Line ends with LF\nLine ends with CR\rLine ends with CRLF\r\nLine without EOL"};
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
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
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{true}, parse_lines{false}} | tags;
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
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{false}, parse_lines{false}} | tags;
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
            mime_type{"text/plain"}, confidence::highest} |
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
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
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
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{false}} |
        tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Line"))),
        VariantWith<tag::BreakLine>(_),
        VariantWith<tag::CloseDocument>(_)
    ));    
}

TEST(HTMLParser, table)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    docwire::data_source{std::string{
        "<table>"
            "<caption>Table caption</caption>"
            "<thead><tr><th>Header 1</th><th>Header 2</th></tr></thead>"
            "<tbody>"
                "<tr><td>Row 1 Cell 1</td><td>Row 1 Cell 2</td></tr>"
                "<tr><td>Row 2 Cell 1</td><td>Row 2 Cell 2</td></tr>"
            "</tbody>"
            "<tfoot><tr><td>Footer 1</td><td>Footer 2</td></tr></tfoot>"
        "</table>"},
        mime_type{"text/html"}, confidence::highest} |
        HTMLParser{} | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Table>(_),
        VariantWith<tag::Caption>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Table caption"))),
        VariantWith<tag::CloseCaption>(_),
        VariantWith<tag::TableRow>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Header 1"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Header 2"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::CloseTableRow>(_),
        VariantWith<tag::TableRow>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Row 1 Cell 1"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Row 1 Cell 2"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::CloseTableRow>(_),
        VariantWith<tag::TableRow>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Row 2 Cell 1"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Row 2 Cell 2"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::CloseTableRow>(_),
        VariantWith<tag::TableRow>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Footer 1"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::TableCell>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Footer 2"))),
        VariantWith<tag::CloseTableCell>(_),
        VariantWith<tag::CloseTableRow>(_),
        VariantWith<tag::CloseTable>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
}

TEST(HTMLParser, whitespaces)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    docwire::data_source{std::string{
        "<div>\n"
            "\t <p> Paragraph </p> \n"
            "\t <p>  Paragraph   with   many   spaces   </p>\n"
            "   <p>Paragraph&nbsp;with&nbsp;non-breaking&nbsp;spaces</p>\n"
        "</div>"},
        mime_type{"text/html"}, confidence::highest} |
        HTMLParser{} | tags;    
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::Section>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph with many spaces"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::Paragraph>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Paragraph\xC2\xA0with\xC2\xA0non-breaking\xC2\xA0spaces"))),
        VariantWith<tag::CloseParagraph>(_),
        VariantWith<tag::CloseSection>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
}

TEST(HTMLParser, encoding)
{
    using namespace testing;
    using namespace chaining;
    std::vector<const char*> test_cases =
    {
        "<html><head><meta charset=\"cp1250\"></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=cp1250\"></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<?xml version=\"1.0\" encoding=\"cp1250\"?><html xmlns=\"http://www.w3.org/1999/xhtml\"><head></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<html><body><p>ąśćłóżł</p></body></html>"
    };
    for (const auto& html_content : test_cases)
    {
        std::vector<Tag> tags;
        docwire::data_source{std::string{html_content},
            mime_type{"text/html"}, confidence::highest} |
            HTMLParser{} | tags;
        ASSERT_THAT(tags, testing::ElementsAre(
            VariantWith<tag::Document>(_),
            VariantWith<tag::Paragraph>(_),
            VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("ąśćłóżł"))),
            VariantWith<tag::CloseParagraph>(_),
            VariantWith<tag::CloseDocument>(_)
        ));
    }
}

TEST(HTMLParser, lists)
{
    using namespace testing;
    using namespace chaining;
    std::vector<Tag> tags;
    docwire::data_source{std::string{
        "<ul>"
            "<li>Item 1</li>"
            "<li>Item 2</li>"
        "</ul>"
        "<ol>"
            "<li>Item 3</li>"
            "<li>Item 4</li>"
        "</ol>"
        "<ul style=\"list-style: none\">"
            "<li>Item 5</li>"
            "<li>Item 6</li>"
        "</ul>"},
        mime_type{"text/html"}, confidence::highest} |
        HTMLParser{} | tags;
    ASSERT_THAT(tags, testing::ElementsAre(
        VariantWith<tag::Document>(_),
        VariantWith<tag::List>(testing::Field(&tag::List::type, StrEq("disc"))),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 1"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 2"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::CloseList>(_),
        VariantWith<tag::List>(testing::Field(&tag::List::type, StrEq("decimal"))),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 3"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 4"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::CloseList>(_),
        VariantWith<tag::List>(testing::Field(&tag::List::type, StrEq("none"))),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 5"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::ListItem>(_),
        VariantWith<tag::Text>(testing::Field(&tag::Text::text, StrEq("Item 6"))),
        VariantWith<tag::CloseListItem>(_),
        VariantWith<tag::CloseList>(_),
        VariantWith<tag::CloseDocument>(_)
    ));
}


TEST(OCRParser, leptonica_stderr_capturer)
{
    try
    {
        using namespace chaining;
        data_source{std::string{"Incorrect image data"}, 
            mime_type{"image/jpeg"}, confidence::highest} |
            OCRParser{} | std::vector<Tag>{};
        FAIL() << "OCRParser should have thrown an exception";
    }
    catch (const std::exception& e)
    {
        ASSERT_TRUE(errors::contains_type<errors::uninterpretable_data>(e))
            << "Thrown exception diagnostic message:\n" << errors::diagnostic_message(e);
        ASSERT_THAT(errors::diagnostic_message(e), testing::HasSubstr(
            "with context \"leptonica_stderr_capturer.contents(): Error in pixReadMem: Unknown format: no pix returned\""));
    }
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

TEST(chaining, set_chain_get_chain_top_chain)
{
    class TestChainElement : public ChainElement
    {
    protected:
        bool is_leaf() const override { return false; }
        void process(Info &info) override {}        
    };

    TestChainElement element0;
    TestChainElement element1;
    TestChainElement element2;
    ASSERT_FALSE(element0.chain());
    ASSERT_FALSE(element1.chain());
    ASSERT_FALSE(element2.chain());
    ParsingChain chain1{element1, element2};
    element0.set_chain(chain1);
    ASSERT_TRUE(element0.chain());
    ASSERT_TRUE(element1.chain());
    ASSERT_TRUE(element2.chain());
    ASSERT_EQ(&element0.chain()->get(), &chain1);
    ASSERT_EQ(&element1.chain()->get(), &chain1);
    ASSERT_EQ(&element2.chain()->get(), &chain1);
    ASSERT_EQ(&element0.chain()->get().top_chain(), &chain1);
    ASSERT_EQ(&element1.chain()->get().top_chain(), &chain1);
    ASSERT_EQ(&element2.chain()->get().top_chain(), &chain1);
    TestChainElement element3;
    ParsingChain chain2{chain1, element3};
    ASSERT_EQ(&chain1.chain()->get(), &chain2);
    ASSERT_EQ(&element1.chain()->get().top_chain(), &chain2);
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

TEST(stringification, enums)
{
    ASSERT_EQ(stringify(confidence::very_high), "very_high");
}
