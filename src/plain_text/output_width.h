/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_PLAIN_TEXT_OUTPUT_WIDTH_H
#define DF0_PLAIN_TEXT_OUTPUT_WIDTH_H

#include "../ranged.h"

namespace df0::plain_text {

/**
 * @brief Strong type representing maximum output line width for plain text.
 *
 * Values are enforced to be at least 20.
 */
class output_width {
public:
    explicit output_width(int w) : m_value(w) {}
    int value() const { return static_cast<int>(m_value); }
private:
    at_least<20, int> m_value;
};

} // namespace df0::plain_text

#endif
