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
        document::document{},
        document::table{},
        document::caption{},
        document::text{.text = "Table caption"},
        document::close_caption{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Header 1"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Header 2"},
        document::close_table_cell{},
        document::close_table_row{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Row 1 Cell 1"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Row 1 Cell 2"},
        document::close_table_cell{},
        document::close_table_row{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Row 2 Cell 1"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Row 2 Cell 2"},
        document::close_table_cell{},
        document::close_table_row{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Footer 1"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Footer 2"},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );
    for (auto& msg : msgs)
    {
        parsing_chain(std::move(msg));
    }
    ASSERT_EQ(output_stream.str(), expected);
}

TEST(html_exporter, table)
{
    test_table_exporting<html_exporter>(
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

TEST(plain_text_exporter, table)
{
    test_table_exporting<plain_text_exporter>(
        "Table caption\n"
        "Header 1      Header 2    \n"
        "Row 1 Cell 1  Row 1 Cell 2\n"
        "Row 2 Cell 1  Row 2 Cell 2\n"
        "Footer 1      Footer 2    \n"
        "\n");
}

TEST(plain_text_exporter, table_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><table><tr><td>table inside table without cells</td></tr></table></table></html>"} |
            html_parser{} |
            plain_text_exporter{} |
            std::ostringstream{}
    );
}

TEST(plain_text_exporter, table_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr><table><tr><td>table inside table without cells</td></tr></table></tr></table></html>"} |
            html_parser{} |
            plain_text_exporter{} |
            std::ostringstream{}
    );
}

TEST(plain_text_exporter, cell_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><thead><td>cell without row</td></thead></table></html>"} |
            html_parser{} |
            plain_text_exporter{} |
            std::ostringstream{}
    );
}

TEST(plain_text_exporter, content_inside_table_without_rows)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table>content without rows</table></html>"} |
            html_parser{} |
            plain_text_exporter{} |
            std::ostringstream{}
    );
}

TEST(plain_text_exporter, content_inside_table_row_without_cells)
{
    ASSERT_ANY_THROW(
        std::string{"<html><table><tr>content without cell</tr></table></html>"} |
            html_parser{} |
            plain_text_exporter{} |
            std::ostringstream{}
    );
}

TEST(plain_text_exporter, eol_sequence_crlf)
{
    plain_text_exporter exporter{eol_sequence{"\r\n"}};
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    std::vector<message_ptr> msgs = make_message_vector
    (
        document::document{},
        document::text{.text = "Line1"},
        document::break_line{},
        document::text{.text = "Line2"},
        document::close_document{}
    );
    for (auto& msg : msgs)
    {
        parsing_chain(std::move(msg));
    }
    ASSERT_EQ(output_stream.str(), "Line1\r\nLine2\r\n");
}

TEST(plain_text_exporter, custom_link_formatting)
{
    plain_text_exporter exporter(
        eol_sequence{"\n"},
        link_formatter{
            .format_opening = [](const document::link& link){ return (link.url ? "(" + *link.url + ")" : "") + "["; },
            .format_closing = [](const document::close_link& link){ return "]"; }
        });
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    std::vector<message_ptr> msgs = make_message_vector
    (
        document::document{},
        document::link{.url = "https://docwire.io"},
        document::text{.text = "DocWire SDK home page"},
        document::close_link{},
        document::close_document{}
    );
    for (auto msg: msgs)
    {
        parsing_chain(std::move(msg));
    }
    ASSERT_EQ(output_stream.str(), "(https://docwire.io)[DocWire SDK home page]\n");
}
