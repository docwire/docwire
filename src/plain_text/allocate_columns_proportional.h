#ifndef DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_PROPORTIONAL_H
#define DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_PROPORTIONAL_H

#include <algorithm>
#include <numeric>
#include <vector>

namespace docwire::plain_text {

/**
 * @brief Allocate column widths proportionally, respecting minimum widths
 *        (normal case: total_min <= usable_width).
 */
inline std::vector<int> allocate_columns_proportional(const std::vector<int>& min_widths,
                                                      const std::vector<int>& desired_widths,
                                                      int usable_width)
{
    const size_t num_cols = min_widths.size();
    if (num_cols == 0)
        return {};
    std::vector<int> col_widths(num_cols, 0);
    int remaining = usable_width;
    for (size_t i = 0; i < num_cols; ++i) {
        col_widths[i] = min_widths[i];
        remaining -= min_widths[i];
    }
    if (remaining > 0) {
        std::vector<int> extras(num_cols, 0);
        for (size_t i = 0; i < num_cols; ++i)
            extras[i] = std::max(0, desired_widths[i] - min_widths[i]);

        int total_extra = std::accumulate(extras.begin(), extras.end(), 0);
        if (total_extra > 0) {
            int allocated = 0;
            for (size_t i = 0; i < num_cols; ++i) {
                int add = (remaining * extras[i]) / total_extra;
                col_widths[i] += add;
                allocated += add;
            }
            for (size_t i = 0; allocated < remaining && i < num_cols; ++i) {
                if (extras[i] > 0) {
                    ++col_widths[i];
                    ++allocated;
                }
            }
        } else {
            int per_col = remaining / num_cols;
            for (size_t i = 0; i < num_cols; ++i)
                col_widths[i] += per_col;
        }
    }
    return col_widths;
}

} // namespace docwire::plain_text

#endif
