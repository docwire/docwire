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

#ifndef DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_AUTO_H
#define DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_AUTO_H

#include "allocate_columns_proportional.h"
#include "allocate_columns_overshoot.h"
#include "fallback_columns.h"
#include <numeric>
#include <utility>
#include <vector>

namespace df0::plain_text {

/**
 * @brief Chooses the best column‑width allocation strategy automatically.
 *
 * @return pair {col_widths, inter_col_gap} – the gap may be set to 0 when
 *         the fallback strategy is used because the window is extremely narrow.
 */
inline std::pair<std::vector<int>, int>
allocate_columns_auto(const std::vector<int>& min_widths,
                      const std::vector<int>& desired_widths,
                      int total_width, int inter_col_gap)
{
    size_t max_cols = min_widths.size();
    if (max_cols == 0)
        return {{}, 0};

    int usable_width = total_width - static_cast<int>(max_cols - 1) * inter_col_gap;
    if (usable_width < static_cast<int>(max_cols))
        return { fallback_columns(max_cols, total_width), 0 };
    int total_min = std::accumulate(min_widths.begin(), min_widths.end(), 0);
    if (total_min > usable_width)
        return { allocate_columns_overshoot(min_widths, usable_width), inter_col_gap };
    return { allocate_columns_proportional(min_widths, desired_widths, usable_width), inter_col_gap };
}

} // namespace df0::plain_text

#endif // DOCWIRE_PLAIN_TEXT_ALLOCATE_COLUMNS_AUTO_H
