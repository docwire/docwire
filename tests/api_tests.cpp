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

#include "office_formats_parser_provider.h"
#include <boost/algorithm/string.hpp>
#include <boost/json.hpp>
#include <future>
#include "gtest/gtest.h"
#include "../src/exception.h"
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
#include "post.h"
#include <regex>
#include "transformer_func.h"
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
		"multilang-spa-ara-lat-grc.png",
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
        std::string test_text {
            "Error processing file " + file_name + ".\n" + ex.what()
        };
        std::replace(test_text.begin(), test_text.end(), '\\', '/');
        
        EXPECT_EQ(test_text, expected_text);
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
	output_val.as_object()["headers"].as_object().erase("x-amzn-trace-id");
	output_val.as_object()["headers"].as_object().erase("user-agent");
    output_val.as_object()["headers"].as_object().erase("x-request-start");
	EXPECT_EQ(read_test_file("http_post.out.json"), serialize(output_val));
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
	std::string output_str = std::regex_replace(output_stream.str(), std::regex("boundary=[^\"]+"), "boundary=<boundary>");
	value output_val = parse(output_str);
	output_val.as_object()["headers"].as_object().erase("x-amzn-trace-id");
	output_val.as_object()["headers"].as_object().erase("user-agent");
    output_val.as_object()["headers"].as_object().erase("x-request-start");

	EXPECT_EQ(read_test_file("http_post_form.out.json"), serialize(output_val));
}

namespace test_ns
{
DOCWIRE_EXCEPTION_DEFINE(TestError1, RuntimeError);
DOCWIRE_EXCEPTION_DEFINE(TestError2, LogicError);
}

TEST(Exceptions, DefiningCreatingAndNested)
{
	std::string what_msg = test_ns::TestError1("msg1", test_ns::TestError2("msg2")).what();
	EXPECT_EQ(what_msg, "msg1 with nested test_ns::TestError2 msg2");
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

TEST(DataSource, reading_seekable_stream)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{seekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    std::string str = data.string();
    ASSERT_EQ(str[0], static_cast<char>(std::byte{0}));
    ASSERT_EQ(str[255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(str[99 * 256], static_cast<char>(std::byte{0}));
    ASSERT_EQ(str[99 * 256 + 255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(str[100 * 256], 't');
}

TEST(DataSource, reading_unseekable_stream)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{unseekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    std::string str = data.string();
    ASSERT_EQ(str[0], static_cast<char>(std::byte{0}));
    ASSERT_EQ(str[255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(str[99 * 256], static_cast<char>(std::byte{0}));
    ASSERT_EQ(str[99 * 256 + 255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(str[100 * 256], 't');
}

TEST(PlainTextExporter, table_inside_table_without_rows)
{
    try
    {
        std::string{"<html><table><table><tr><td>table inside table without cells</td></tr></table></table></html>"} |
            ParseDetectedFormat<OfficeFormatsParserProvider>{} |
            PlainTextExporter{} |
            std::ostringstream{};
        FAIL() << "LogicError exception was expected.";
    }
    catch (const LogicError& error)
    {
        ASSERT_EQ(error.what(), std::string{"Table inside table without rows."});
    }
}

TEST(PlainTextExporter, table_inside_table_row_without_cells)
{
    try
    {
        std::string{"<html><table><tr><table><tr><td>table inside table without cells</td></tr></table></tr></table></html>"} |
            ParseDetectedFormat<OfficeFormatsParserProvider>{} |
            PlainTextExporter{} |
            std::ostringstream{};
        FAIL() << "LogicError exception was expected.";
    }
    catch (const LogicError& error)
    {
        ASSERT_EQ(error.what(), std::string{"Table inside table row without cells."});
    }
}

TEST(PlainTextExporter, cell_inside_table_without_rows)
{
    try
    {
        std::string{"<html><table><thead><td>cell without row</td></thead></table></html>"} |
            ParseDetectedFormat<OfficeFormatsParserProvider>{} |
            PlainTextExporter{} |
            std::ostringstream{};
        FAIL() << "LogicError exception was expected.";
    }
    catch (const LogicError& error)
    {
        ASSERT_EQ(error.what(), std::string{"Cell inside table without rows."});
    }
}

TEST(PlainTextExporter, content_inside_table_without_rows)
{
    try
    {
        std::string{"<html><table>content without rows</table></html>"} |
            ParseDetectedFormat<OfficeFormatsParserProvider>{} |
            PlainTextExporter{} |
            std::ostringstream{};
        FAIL() << "LogicError exception was expected.";
    }
    catch (const LogicError& error)
    {
        ASSERT_EQ(error.what(), std::string{"Cell content inside table without rows."});
    }
}

TEST(PlainTextExporter, content_inside_table_row_without_cells)
{
    try
    {
        std::string{"<html><table><tr>content without cell</tr></table></html>"} |
            ParseDetectedFormat<OfficeFormatsParserProvider>{} |
            PlainTextExporter{} |
            std::ostringstream{};
        FAIL() << "LogicError exception was expected.";
    }
    catch (const LogicError& error)
    {
        ASSERT_EQ(error.what(), std::string{"Cell content inside table row without cells."});
    }
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
    path | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, path_temp)
{
    std::ostringstream output_stream{};    
    std::filesystem::path{"1.doc"} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_ref)
{
    std::ostringstream output_stream{};
    std::string str = read_test_file("1.doc");
    str | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, string_temp)
{
    std::ostringstream output_stream{};    
    read_test_file("1.doc") | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, seekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    seekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_ref)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)};
    stream_ptr | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, unseekable_stream_ptr_temp)
{
    std::ostringstream output_stream{};
    unseekable_stream_ptr{std::make_shared<std::ifstream>("1.doc", std::ios_base::binary)} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_shared_ptr)
{
    std::ostringstream output_stream{};
    std::make_shared<std::ifstream>("1.doc", std::ios_base::binary) | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}

TEST(Input, stream_temp)
{
    std::ostringstream output_stream{};
    std::ifstream{"1.doc", std::ios_base::binary} | ParseDetectedFormat<OfficeFormatsParserProvider>{} | PlainTextExporter{} | output_stream;
    ASSERT_EQ(output_stream.str(), read_test_file("1.doc.out"));
}
