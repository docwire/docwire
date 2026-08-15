#ifndef DOCWIRE_PLAIN_TEXT_COMPUTE_COLUMN_WIDTH_RANGES_H
#define DOCWIRE_PLAIN_TEXT_COMPUTE_COLUMN_WIDTH_RANGES_H

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace docwire::plain_text {

/**
 * @brief Computes minimum and desired column widths from raw cell lines.
 *
 * Minimum width is the longest word in a column; desired width is the longest
 * unwrapped line in that column.
 *
 * @param raw_cell_lines raw_cell_lines[row][col] is the list of unwrapped lines for a cell.
 * @param max_cols       number of columns in the table.
 * @return               pair {min_widths, desired_widths} (both vectors of length max_cols).
 */
inline std::pair<std::vector<int>, std::vector<int>>
compute_column_width_ranges(const std::vector<std::vector<std::vector<std::string>>>& raw_cell_lines,
                            size_t max_cols)
{
    std::vector<int> min_widths(max_cols, 0);
    std::vector<int> desired_widths(max_cols, 0);
    for (size_t col = 0; col < max_cols; ++col) {
        int min_w = 0;
        int desired_w = 0;
        for (const auto& row : raw_cell_lines) {
            if (col < row.size()) {
                for (const auto& line : row[col]) {
                    desired_w = std::max(desired_w, static_cast<int>(line.size()));
                    std::istringstream iss(line);
                    std::string word;
                    while (iss >> word)
                        min_w = std::max(min_w, static_cast<int>(word.size()));
                }
            }
        }
        min_widths[col] = min_w;
        desired_widths[col] = desired_w;
    }
    return {min_widths, desired_widths};
}

} // namespace docwire::plain_text

#endif
