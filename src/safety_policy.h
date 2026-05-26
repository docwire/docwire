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

#ifndef DOCWIRE_SAFETY_POLICY_H
#define DOCWIRE_SAFETY_POLICY_H

namespace docwire
{

/**
 * @brief Defines the safety policy for operations.
 */
enum class safety_policy { 
    /// Perform runtime checks and throw exceptions on violations.
    strict, 
    /// Skip runtime checks for performance; undefined behavior on violations.
    relaxed 
};
using enum safety_policy;
constexpr inline safety_policy default_safety_level = strict;

}

#endif // DOCWIRE_SAFETY_POLICY_H
