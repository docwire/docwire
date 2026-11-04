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

#ifndef DF0_CONCEPTS_STREAM_H
#define DF0_CONCEPTS_STREAM_H

#include <concepts>
#include <iosfwd>

namespace df0
{

/**
 * @brief Concept for types that are streamable to `std::ostream`.
 */
template <typename T>
concept streamable = requires (std::ostream& os, const T& value) { { os << value } -> std::convertible_to<std::ostream&>; };

} // namespace df0

#endif // DF0_CONCEPTS_STREAM_H
