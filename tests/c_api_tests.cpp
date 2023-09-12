/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

extern "C" {
    #include "stdio.h"
    #include "../src/doctotext_c_api.h"
}
#include "gtest/gtest.h"
#include <algorithm>
#include <string_view>
#include <tuple>
#include <fstream>
#include <iterator>
#include <array>

class HTMLWriteTestC : public ::testing::TestWithParam<const char*>
{
};

TEST_P(HTMLWriteTestC, CApiTest)
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
    std::string temp_file_name{ std::string{ name } + ".tmp" };
    FILE* temp_fptr = fopen(temp_file_name.c_str(), "w");
    
    DocToTextParserManager* parser_manager = doctotext_init_parser_manager("");

    DocToTextInput *input = doctotext_create_input_from_file_name(file_name.c_str());
    DocToTextImporter *importer = doctotext_create_importer(parser_manager);
    DocToTextExporter *exporter = doctotext_create_html_exporter(temp_fptr);
    DocToTextParsingChain *parsing_chain = doctotext_connect_parsing_chain_to_exporter(doctotext_connect_input_to_importer(input, importer), exporter);
    fclose(temp_fptr);

    std::ifstream parsed_ifs{ temp_file_name };
    std::string parsed_text{ std::istreambuf_iterator<char>{parsed_ifs},
        std::istreambuf_iterator<char>{} };
 
    // THEN
    EXPECT_EQ(expected_text, parsed_text);
}

INSTANTIATE_TEST_SUITE_P(
    SimpleExtractorHTMLTest, HTMLWriteTestC,
    ::testing::Values(
        "10.docx"
                      ),
    [](const ::testing::TestParamInfo<HTMLWriteTestC::ParamType>& info) {
        std::string file_name = info.param;
        std::transform(file_name.cbegin(), file_name.cend(), file_name.begin(), [](const auto ch)
        {   if(ch == '.') return '_'; 
            else return ch; });

        std::string name = file_name + "_basic_c_api_html_tests";
        return name;
    });
