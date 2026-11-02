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

#ifndef DOCWIRE_AUDIT_AUDIT_TAG_H
#define DOCWIRE_AUDIT_AUDIT_TAG_H

#include <concepts>
#include <string_view>

namespace docwire::audit
{

/**
 * @brief Concept for types that behave as audit tags.
 *
 * Audit tags identify the semantic category of an audit event in a
 * structured, type-safe way.
 */
template<typename T>
concept audit_tag = requires
{
    { T::string() } -> std::convertible_to<std::string_view>;
};

} // namespace docwire::audit

#endif
