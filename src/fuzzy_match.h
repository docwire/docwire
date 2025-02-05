/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_FUZZY_MATCH_H
#define DOCWIRE_FUZZY_MATCH_H

#include "export.h"
#include <string>

namespace docwire::fuzzy_match
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
DOCWIRE_EXPORT double ratio(const std::string& a, const std::string& b);

} // namespace docwire::fuzzy_match

#endif // DOCWIRE_FUZZY_MATCH_H
