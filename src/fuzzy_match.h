/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_FUZZY_MATCH_H
#define DF0_FUZZY_MATCH_H

#include "fuzzy_match_export.h"
#include <string>

namespace df0::fuzzy_match
{

/**
 * @brief Calculates a ratio of similarity between two strings.
 *
 * This function calculates a ratio of similarity between two strings.
 * The ratio is a value in the range [0, 100], where 0 represents absolutely no similarity and
 * 100 represents absolute similarity.
 *
 * @param a The first string to compare.
 * @param b The second string to compare.
 *
 * @return The ratio of similarity between the two strings.
 */
DF0_FUZZY_MATCH_EXPORT double ratio(const std::string& a, const std::string& b);

} // namespace df0::fuzzy_match

#endif // DF0_FUZZY_MATCH_H
