#ifndef DOCWIRE_PLAIN_TEXT_RENDER_TABLE_H
#define DOCWIRE_PLAIN_TEXT_RENDER_TABLE_H

#include <algorithm>
#include <string>
#include <vector>

namespace docwire::plain_text {

/**
 * @brief Renders a plain‑text table from already wrapped cell content.
 *
 * Reference conceptual model: w3m terminal table layout and Pandoc plain writer.
 * Empty rows (all cells contain no visible characters) are omitted, as they
 * carry no content in monospaced output.
 */
inline std::string render_table(const std::vector<std::vector<std::vector<std::string>>>& cell_lines,
                                const std::vector<int>& col_widths,
                                int inter_col_gap,
                                const std::string& eol)
{
    std::string out;
    for (const auto& row : cell_lines) {
        // Skip rows where all cells are empty
        bool row_has_content = false;
        for (const auto& col_lines : row) {
            for (const auto& line : col_lines) {
                if (line.find_first_not_of(' ') != std::string::npos) {
                    row_has_content = true;
                    break;
                }
            }
            if (row_has_content)
                break;
        }
        if (!row_has_content)
            continue;

        size_t max_lines = 1;
        for (const auto& col_lines : row)
            max_lines = std::max(max_lines, col_lines.size());

        for (size_t l = 0; l < max_lines; ++l) {
            std::string line;
            for (size_t j = 0; j < row.size(); ++j) {
                std::string content = (l < row[j].size()) ? row[j][l] : "";
                if (j < row.size() - 1) {
                    content.resize(col_widths[j], ' ');
                    line += content;
                    line += std::string(inter_col_gap, ' ');
                } else {
                    line += content;            // last column – no trailing padding
                }
            }
            out += line + eol;
        }
    }
    return out;
}

} // namespace docwire::plain_text

#endif // DOCWIRE_PLAIN_TEXT_RENDER_TABLE_H
