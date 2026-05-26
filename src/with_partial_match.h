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

#ifndef DOCWIRE_WITH_PARTIAL_MATCH_H
#define DOCWIRE_WITH_PARTIAL_MATCH_H

#include <string_view>

namespace with
{

/**
 * @brief A wrapper to indicate that partial matching is allowed during conversion.
 */
struct partial_match
{
    std::string_view v;
    operator std::string_view() const noexcept { return v; }
};

} // namespace with

#endif // DOCWIRE_WITH_PARTIAL_MATCH_H
