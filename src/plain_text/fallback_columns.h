/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_PLAIN_TEXT_FALLBACK_COLUMNS_H
#define DF0_PLAIN_TEXT_FALLBACK_COLUMNS_H

#include <vector>

namespace df0::plain_text {

/**
 * @brief Fallback column allocation for extremely narrow windows.
 *
 * Gives each column at least one character, then distributes any remaining
 * width (if total_width >= num_cols) one per column until exhausted. In the
 * pathological case total_width < num_cols the sum of widths may exceed
 * total_width; that is the only way to avoid zero-width columns.
 */
inline std::vector<int> fallback_columns(int num_cols, int total_width)
{
    if (num_cols <= 0)
        return {};

    std::vector<int> widths(num_cols, 1);
    int remaining = total_width - num_cols;
    while (remaining > 0) {
        for (int i = 0; i < num_cols && remaining > 0; ++i) {
            ++widths[i];
            --remaining;
        }
    }
    return widths;
}

} // namespace df0::plain_text

#endif
