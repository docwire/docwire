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

#include "message_matchers.h"
#include "html_parser.h"
#include "input.h"
#include "output.h"

using namespace docwire;
using namespace testing;

TEST(html_parser, table)
{
    std::vector<message_ptr> msgs;
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
        html_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::table>(_),
        MessagePtrWith<document::caption>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Table caption"))),
        MessagePtrWith<document::close_caption>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Header 1"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Header 2"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Row 1 Cell 1"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Row 1 Cell 2"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Row 2 Cell 1"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Row 2 Cell 2"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Footer 1"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Footer 2"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::close_table>(_),
        MessagePtrWith<document::close_document>(_)
    ));
}

TEST(html_parser, whitespaces)
{
    std::vector<message_ptr> msgs;
    docwire::data_source{std::string{
        "<div>\n"
            "\t <p> Paragraph </p> \n"
            "\t <p>  Paragraph   with   many   spaces   </p>\n"
            "   <p>Paragraph&nbsp;with&nbsp;non-breaking&nbsp;spaces</p>\n"
        "</div>\n"
        "<table>\n"
            "\t<caption> Table caption </caption>\n"
            "\t<tr>\n"
                "\t\t<td> Table cell </td>\n"
                "\t</tr>\n"
        "</table>\n"},
        mime_type{"text/html"}, confidence::highest} |
        html_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::section>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph with many spaces"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph\xC2\xA0with\xC2\xA0non-breaking\xC2\xA0spaces"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::close_section>(_),
        MessagePtrWith<document::table>(_),
        MessagePtrWith<document::caption>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Table caption"))),
        MessagePtrWith<document::close_caption>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Table cell"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::close_table>(_),
        MessagePtrWith<document::close_document>(_)
    ));
}

TEST(html_parser, encoding)
{
    std::vector<const char*> test_cases =
    {
        "<html><head><meta charset=\"cp1250\"></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=cp1250\"></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<?xml version=\"1.0\" encoding=\"cp1250\"?><html xmlns=\"http://www.w3.org/1999/xhtml\"><head></head><body><p>\xB9\x9C\xE6\xB3\xF3\xBF\xB3</p></body></html>",
        "<html><body><p>ąśćłóżł</p></body></html>"
    };
    for (const auto& html_content : test_cases)
    {
        std::vector<message_ptr> msgs;
        docwire::data_source{std::string{html_content},
            mime_type{"text/html"}, confidence::highest} |
            html_parser{} | msgs;
        ASSERT_THAT(msgs, ElementsAre(
            MessagePtrWith<document::document>(_),
            MessagePtrWith<document::paragraph>(_),
            MessagePtrWith<document::text>(Field(&document::text::text, StrEq("ąśćłóżł"))),
            MessagePtrWith<document::close_paragraph>(_),
            MessagePtrWith<document::close_document>(_)
        ));
    }
}

TEST(html_parser, lists)
{
    std::vector<message_ptr> msgs;
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
        html_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::list>(Field(&document::list::type, StrEq("disc"))),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 1"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 2"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::close_list>(_),
        MessagePtrWith<document::list>(Field(&document::list::type, StrEq("decimal"))),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 3"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 4"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::close_list>(_),
        MessagePtrWith<document::list>(Field(&document::list::type, StrEq("none"))),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 5"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::list_item>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Item 6"))),
        MessagePtrWith<document::close_list_item>(_),
        MessagePtrWith<document::close_list>(_),
        MessagePtrWith<document::close_document>(_)
    ));
}

TEST(html_parser, misplaced_tags)
{
    std::vector<message_ptr> msgs;
    docwire::data_source{std::string{
        "<html>\n"
        "\t<body>\n"
        "\t\t<title>title1</title>\n"
        "\t\t<table>\n"
        "\t\t<title>title2</title>\n"
        "\t\t<style>css content</style>\n"
        "\t\t<tr><td>cell1</td></tr>\n"
        "\t\t<p>paragraph1</p>\n"
        "\t\t<tr><p>paragraph2</p><td>cell2</td></tr>\n"
        "\t</table>\n"
        "\t</body>\n"
        "</html>\n"},
        mime_type{"text/html"}, confidence::highest} |
        html_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("paragraph1"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("paragraph2"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::style>(Field(&document::style::css_text, StrEq("css content"))),
        MessagePtrWith<document::table>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("cell1"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::table_row>(_),
        MessagePtrWith<document::table_cell>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("cell2"))),
        MessagePtrWith<document::close_table_cell>(_),
        MessagePtrWith<document::close_table_row>(_),
        MessagePtrWith<document::close_table>(_),
        MessagePtrWith<document::close_document>(_)
    ));
}
