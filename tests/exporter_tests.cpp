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
#include "gtest/gtest.h"
#include "html_exporter.h"
#include "input.h"
#include <magic_enum/magic_enum_iostream.hpp>
#include <optional>
#include "html_parser.h"
#include "output.h"
#include "plain_text_exporter.h"

using namespace docwire;

namespace
{
template<typename... T>
std::vector<docwire::message_ptr> make_message_vector(T&&... args)
{
    std::vector<docwire::message_ptr> vec;
    vec.reserve(sizeof...(args));
    (vec.push_back(std::make_shared<docwire::message<std::decay_t<T>>>(std::forward<T>(args))), ...);
    return vec;
}
} // anonymous namespace

template<typename Exporter>
void test_table_exporting(const std::string& expected)
{
    std::ostringstream output_stream{};
    auto parsing_chain = Exporter{} | output_stream;
    auto msgs = make_message_vector
    (
        document::Document{},
        document::Table{},
        document::Caption{},
        document::Text{.text = "Table caption"},
        document::CloseCaption{},
        document::TableRow{},
        document::TableCell{},
        document::Text{.text = "Header 1"},
        document::CloseTableCell{},
        document::TableCell{},
        document::Text{.text = "Header 2"},
        document::CloseTableCell{},
        document::CloseTableRow{},
        document::TableRow{},
        document::TableCell{},
        document::Text{.text = "Row 1 Cell 1"},
        document::CloseTableCell{},
        document::TableCell{},
        document::Text{.text = "Row 1 Cell 2"},
        document::CloseTableCell{},
        document::CloseTableRow{},
        document::TableRow{},
        document::TableCell{},
        document::Text{.text = "Row 2 Cell 1"},
        document::CloseTableCell{},
        document::TableCell{},
        document::Text{.text = "Row 2 Cell 2"},
        document::CloseTableCell{},
        document::CloseTableRow{},
        document::TableRow{},
        document::TableCell{},
        document::Text{.text = "Footer 1"},
        document::CloseTableCell{},
        document::TableCell{},
        document::Text{.text = "Footer 2"},
        document::CloseTableCell{},
        document::CloseTableRow{},
        document::CloseTable{},
        document::CloseDocument{}
    );
    for (auto& msg : msgs)
    {
        parsing_chain(std::move(msg));
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
    std::vector<message_ptr> msgs = make_message_vector
    (
        document::Document{},
        document::Text{.text = "Line1"},
        document::BreakLine{},
        document::Text{.text = "Line2"},
        document::CloseDocument{}
    );
    for (auto& msg : msgs)
    {
        parsing_chain(std::move(msg));
    }
    ASSERT_EQ(output_stream.str(), "Line1\r\nLine2\r\n");
}

TEST(PlainTextExporter, custom_link_formatting)
{
    PlainTextExporter exporter(
        eol_sequence{"\n"},
        link_formatter{
            .format_opening = [](const document::Link& link){ return (link.url ? "(" + *link.url + ")" : "") + "["; },
            .format_closing = [](const document::CloseLink& link){ return "]"; }
        });
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    std::vector<message_ptr> msgs = make_message_vector
    (
        document::Document{},
        document::Link{.url = "https://docwire.io"},
        document::Text{.text = "DocWire SDK home page"},
        document::CloseLink{},
        document::CloseDocument{}
    );
    for (auto msg: msgs)
    {
        parsing_chain(std::move(msg));
    }
    ASSERT_EQ(output_stream.str(), "(https://docwire.io)[DocWire SDK home page]\n");
}
