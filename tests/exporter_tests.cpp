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
#include <numeric>
#include <algorithm>
#include "html_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "plain_text/wrap.h"
#include "plain_text/wrap_lines.h"
#include "plain_text/compute_column_width_ranges.h"
#include "plain_text/allocate_columns_proportional.h"
#include "plain_text/allocate_columns_overshoot.h"
#include "plain_text/fallback_columns.h"
#include "plain_text/allocate_columns_auto.h"
#include "plain_text/render_table.h"

using namespace docwire;
using docwire::plain_text::output_width;

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
template<typename Exporter>
void test_table_exporting(Exporter&& exporter, const std::string& expected)
{
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
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
        "Header 1                                 Header 2\n"
        "Row 1 Cell 1                             Row 1 Cell 2\n"
        "Row 2 Cell 1                             Row 2 Cell 2\n"
        "Footer 1                                 Footer 2\n"
        "\n");
}

TEST(plain_text_exporter, table_wide_output)
{
    const int width = 200;
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{width});

    // Expected column widths for 2 columns: usable=198, min=6, desired=13 => col_width=99
    const int col_width = 99;
    const int gap = 2;

    // Build expected string dynamically to guarantee exact match
    std::string expected = "Table caption\n";
    auto cell_line = [&](const std::string& left, const std::string& right) {
        std::string line;
        line += left;
        line += std::string(col_width - left.size(), ' ');
        line += std::string(gap, ' ');
        line += right;
        line += '\n';
        return line;
    };

    expected += cell_line("Header 1", "Header 2");
    expected += cell_line("Row 1 Cell 1", "Row 1 Cell 2");
    expected += cell_line("Row 2 Cell 1", "Row 2 Cell 2");
    expected += cell_line("Footer 1", "Footer 2");
    expected += '\n';  // trailing newline after table

    test_table_exporting<plain_text_exporter>(std::move(exporter), expected);
}

TEST(plain_text_exporter, table_caption_wrapped_by_output_width)
{
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{40});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;

    std::string long_caption(100, 'x');

    auto msgs = make_message_vector
    (
        document::document{},
        document::table{},
        document::caption{},
        document::text{.text = long_caption},
        document::close_caption{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "cell"},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );

    for (auto& msg : msgs)
        parsing_chain(std::move(msg));

    std::string result = output_stream.str();

    // No line may exceed the configured width (caption included)
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line))
        EXPECT_LE(line.size(), 40) << "Line too long: " << line;

    // All 100 'x' characters from the caption must be retained.
    EXPECT_EQ(std::count(result.begin(), result.end(), 'x'), 100);
}

TEST(plain_text_exporter, table_variable_rows_long_content)
{
    // Simulates an ODS table with variable column counts and a cell containing a very long string.
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{80});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;

    std::string long_text(2000, 'x');   // a 2000-character word

    auto msgs = make_message_vector
    (
        document::document{},
        document::table{},
        // Row 1: two cells, second cell is huge
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Short"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = long_text},
        document::close_table_cell{},
        document::close_table_row{},
        // Row 2: five cells (to test column count mismatch)
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Col1"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Col2"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Col3"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Col4"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = "Col5"},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );

    for (auto& msg : msgs)
        parsing_chain(std::move(msg));

    std::string result = output_stream.str();

    // No line may be longer than the requested width
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line))
        EXPECT_LE(line.size(), 80) << "Line too long: " << line;

    // The entire 2000-character word must be present, even if wrapped.
    // Count every 'x' in the result; any truncation would reduce the count.
    EXPECT_EQ(std::count(result.begin(), result.end(), 'x'), 2000);
}

TEST(plain_text_exporter, nested_table_respects_output_width)
{
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{100});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;

    // Build a table containing a nested table inside one cell.
    auto msgs = make_message_vector
    (
        document::document{},
        document::table{},
        document::table_row{},
        document::table_cell{},
        // Nested table
        document::table{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = std::string(200, 'x')},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );

    for (auto& msg : msgs)
        parsing_chain(std::move(msg));

    std::string result = output_stream.str();
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line))
        EXPECT_LE(line.size(), 100) << "Line too long: " << line;
}

TEST(plain_text_exporter, table_long_url_preserves_content)
{
    // Regression test for customer issue where long URLs inside table cells
    // were truncated by the old 1000-character column limit.
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{80});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;

    std::string long_url = "https://example.com/";
    // Build a URL of exactly 500 characters (no spaces)
    while (long_url.size() < 500)
        long_url += "a";

    auto msgs = make_message_vector
    (
        document::document{},
        document::table{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Short"},
        document::close_table_cell{},
        document::table_cell{},
        document::text{.text = long_url},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );

    for (auto& msg : msgs)
        parsing_chain(std::move(msg));

    std::string result = output_stream.str();

    // No line may exceed the configured width
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line))
        EXPECT_LE(line.size(), 80) << "Line too long: " << line;

    // Every character of the long URL must be retained.
    // Count all 'a' characters in the output: one is in the fixed hostname
    // ("example.com"), and the remaining 480 come from the appended padding.
    const int total_a_expected = 481;
    EXPECT_EQ(std::count(result.begin(), result.end(), 'a'), total_a_expected);
}

TEST(plain_text_exporter, table_narrow_width_wrapping)
{
    plain_text_exporter exporter(eol_sequence{"\n"}, link_formatter{}, output_width{30});
    std::ostringstream output_stream{};
    auto parsing_chain = exporter | output_stream;
    auto msgs = make_message_vector
    (
        document::document{},
        document::table{},
        document::caption{},
        document::text{.text = "Test caption"},
        document::close_caption{},
        document::table_row{},
        document::table_cell{},
        document::text{.text = "Very long cell content that must be wrapped"},
        document::close_table_cell{},
        document::close_table_row{},
        document::close_table{},
        document::close_document{}
    );
    for (auto& msg : msgs)
        parsing_chain(std::move(msg));
    std::string result = output_stream.str();
    // The result should have the caption and then the cell wrapped across multiple lines
    // Verify that no line is longer than 30 characters (excluding newline)
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line))
        EXPECT_LE(line.size(), 30) << "Line too long: " << line;
    // Check that the full original text appears across multiple lines
    EXPECT_TRUE(result.find("Very long cell content that") != std::string::npos);
    EXPECT_TRUE(result.find("must be wrapped") != std::string::npos);
}

TEST(plain_text_exporter, output_width_too_small)
{
    // Should throw because output_width requires >= 20
    EXPECT_ANY_THROW(output_width{10});
    // Also construction of exporter with that width should fail
    EXPECT_ANY_THROW((plain_text_exporter{eol_sequence{"\n"}, link_formatter{}, output_width{10}}));
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

TEST(plain_text_layout, wrap)
{
    using namespace docwire::plain_text;

    // empty input -> one empty line
    auto wrapped = wrap("", 5);
    ASSERT_EQ(wrapped.size(), 1);
    EXPECT_TRUE(wrapped[0].empty());

    // short text stays on one line
    wrapped = wrap("Hello world", 20);
    ASSERT_EQ(wrapped.size(), 1);
    EXPECT_EQ(wrapped[0], "Hello world");

    // wraps at word boundaries
    wrapped = wrap("Hello world", 6);
    ASSERT_EQ(wrapped.size(), 2);
    EXPECT_EQ(wrapped[0], "Hello");
    EXPECT_EQ(wrapped[1], "world");

    // long word with break_long_words=true (default)
    wrapped = wrap("abcdefgh", 4);
    ASSERT_EQ(wrapped.size(), 2);
    EXPECT_EQ(wrapped[0], "abcd");
    EXPECT_EQ(wrapped[1], "efgh");

    // long word with break_long_words=false stays unbroken
    wrapped = wrap("abcdefgh", 4, false);
    ASSERT_EQ(wrapped.size(), 1);
    EXPECT_EQ(wrapped[0], "abcdefgh");
}

TEST(plain_text_layout, wrap_rejects_nonpositive_width)
{
    using namespace docwire::plain_text;
    EXPECT_THROW(wrap("hello", 0), std::invalid_argument);
    EXPECT_THROW(wrap("hello", -1), std::invalid_argument);
}

TEST(plain_text_layout, wrap_lines)
{
    using namespace docwire::plain_text;

    auto lines = wrap_lines(std::vector<std::string>{}, 10);
    ASSERT_EQ(lines.size(), 1);
    EXPECT_TRUE(lines[0].empty());

    std::vector<std::string> input = {"hello world", "short", ""};
    auto wrapped = wrap_lines(input, 6);
    ASSERT_EQ(wrapped.size(), 4);
    EXPECT_EQ(wrapped[0], "hello");
    EXPECT_EQ(wrapped[1], "world");
    EXPECT_EQ(wrapped[2], "short");
    EXPECT_EQ(wrapped[3], "");
}

TEST(plain_text_layout, compute_column_width_ranges)
{
    using namespace docwire::plain_text;
    std::vector<std::vector<std::vector<std::string>>> raw = {
        { {"Header 1"}, {"Header 2"}, {"Header 3"} },
        { {"Row 1 Cell 1"}, {"Row 1 Cell 2"}, {} }
    };
    auto [min_w, desired_w] = compute_column_width_ranges(raw, 3);
    ASSERT_EQ(min_w.size(), 3);
    ASSERT_EQ(desired_w.size(), 3);
    EXPECT_EQ(min_w[0], 6);
    EXPECT_EQ(desired_w[0], 12);
    EXPECT_EQ(min_w[1], 6);
    EXPECT_EQ(desired_w[1], 12);
    EXPECT_EQ(min_w[2], 6);
    EXPECT_EQ(desired_w[2], 8);
}

TEST(plain_text_layout, allocate_columns_proportional)
{
    using namespace docwire::plain_text;
    std::vector<int> min_widths = {6, 6};
    std::vector<int> desired_widths = {13, 13};
    auto widths = allocate_columns_proportional(min_widths, desired_widths, 198);
    ASSERT_EQ(widths.size(), 2);
    EXPECT_EQ(std::accumulate(widths.begin(), widths.end(), 0), 198);
    EXPECT_EQ(widths[0], 99);
    EXPECT_EQ(widths[1], 99);
}

TEST(plain_text_layout, allocate_columns_proportional_empty)
{
    using namespace docwire::plain_text;
    auto widths = allocate_columns_proportional({}, {}, 10);
    EXPECT_TRUE(widths.empty());
}

TEST(plain_text_layout, allocate_columns_overshoot)
{
    using namespace docwire::plain_text;
    std::vector<int> min_widths = {5, 2000, 4, 4, 4};
    auto widths = allocate_columns_overshoot(min_widths, 72);
    ASSERT_EQ(widths.size(), 5);
    EXPECT_EQ(std::accumulate(widths.begin(), widths.end(), 0), 72);
    EXPECT_GT(widths[1], 50);
    for (int w : widths)
        EXPECT_GE(w, 1);
}

TEST(plain_text_layout, fallback_columns)
{
    using namespace docwire::plain_text;
    // Normal case: total_width >= num_cols
    auto widths = fallback_columns(3, 10);
    ASSERT_EQ(widths.size(), 3);
    EXPECT_EQ(widths[0], 4);
    EXPECT_EQ(widths[1], 3);
    EXPECT_EQ(widths[2], 3);
    EXPECT_EQ(std::accumulate(widths.begin(), widths.end(), 0), 10);

    // Narrow case: each column must still be at least 1
    auto narrow = fallback_columns(3, 2);
    ASSERT_EQ(narrow.size(), 3);
    for (int w : narrow)
        EXPECT_GT(w, 0);
    EXPECT_EQ(narrow[0], 1);
    EXPECT_EQ(narrow[1], 1);
    EXPECT_EQ(narrow[2], 1);

    // Single column edge case
    auto single = fallback_columns(1, 5);
    ASSERT_EQ(single.size(), 1);
    EXPECT_EQ(single[0], 5);
}

TEST(plain_text_layout, fallback_columns_zero_or_negative)
{
    using namespace docwire::plain_text;
    EXPECT_TRUE(fallback_columns(0, 10).empty());
    EXPECT_TRUE(fallback_columns(-1, 10).empty());
}

TEST(plain_text_layout, allocate_columns_auto)
{
    using namespace docwire::plain_text;
    std::vector<int> min_widths;
    std::vector<int> desired_widths;

    min_widths = {6, 6};
    desired_widths = {13, 13};
    auto [w1, gap1] = allocate_columns_auto(min_widths, desired_widths, 1, 2);
    EXPECT_EQ(w1.size(), 2);
    EXPECT_EQ(gap1, 0);
    EXPECT_GT(w1[0], 0);
    EXPECT_GT(w1[1], 0);

    min_widths = {5, 2000, 4};
    desired_widths = {5, 2000, 4};
    auto [w2, gap2] = allocate_columns_auto(min_widths, desired_widths, 80, 2);
    EXPECT_EQ(gap2, 2);
    EXPECT_EQ(std::accumulate(w2.begin(), w2.end(), 0), 80 - 2 * static_cast<int>(w2.size() - 1));

    min_widths = {6, 6};
    desired_widths = {13, 13};
    auto [w3, gap3] = allocate_columns_auto(min_widths, desired_widths, 198, 2);
    EXPECT_EQ(gap3, 2);
    EXPECT_EQ(w3[0], 98);
    EXPECT_EQ(w3[1], 98);
}

TEST(plain_text_layout, allocate_columns_auto_fallback_boundary)
{
    using namespace docwire::plain_text;
    // total_width=20, 10 columns, gap=2 -> usable_width=2 < max_cols=10,
    // should use fallback and drop gap.
    std::vector<int> min_widths(10, 1);
    std::vector<int> desired_widths(10, 1);
    auto [w, gap] = allocate_columns_auto(min_widths, desired_widths, 20, 2);
    EXPECT_EQ(gap, 0);
    EXPECT_EQ(std::accumulate(w.begin(), w.end(), 0), 20);
    for (int width : w)
        EXPECT_GT(width, 0);
}

TEST(plain_text_layout, render_table_skips_empty_rows)
{
    using namespace docwire::plain_text;
    std::vector<std::vector<std::vector<std::string>>> cell_lines = {
        { {"Header 1"} },
        { {} },
        { {"Row 1"} }
    };
    std::vector<int> col_widths = {10};
    std::string result = render_table(cell_lines, col_widths, 2, "\n");
    EXPECT_EQ(result, "Header 1\nRow 1\n");
}
