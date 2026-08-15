#ifndef DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_OVERSHOOT_H
#define DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_OVERSHOOT_H

#include <algorithm>
#include <numeric>
#include <vector>

namespace docwire::plain_text {

/**
 * @brief Allocate column widths when minimum widths overflow usable space
 *        (total_min > usable_width).  Columns are scaled down proportionally.
 */
inline std::vector<int> allocate_columns_overshoot(const std::vector<int>& min_widths,
                                                   int usable_width)
{
    const size_t num_cols = min_widths.size();
    std::vector<int> col_widths(num_cols, 0);
    int total_min = std::accumulate(min_widths.begin(), min_widths.end(), 0);
    for (size_t i = 0; i < num_cols; ++i) {
        col_widths[i] = std::max(1, (min_widths[i] * usable_width) / total_min);
    }
    int sum = std::accumulate(col_widths.begin(), col_widths.end(), 0);
    int diff = usable_width - sum;
    if (diff > 0) {
        for (size_t i = 0; diff > 0 && i < num_cols; ++i) {
            ++col_widths[i];
            --diff;
        }
    } else if (diff < 0) {
        while (diff < 0) {
            int max_idx = 0;
            int max_val = 0;
            for (size_t i = 0; i < num_cols; ++i) {
                if (col_widths[i] > max_val && col_widths[i] > 1) {
                    max_val = col_widths[i];
                    max_idx = i;
                }
            }
            if (max_val <= 1) break;
            --col_widths[max_idx];
            ++diff;
        }
    }
    return col_widths;
}

} // namespace docwire::plain_text

#endif
