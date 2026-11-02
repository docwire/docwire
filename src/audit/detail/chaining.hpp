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

#ifndef DOCWIRE_AUDIT_DETAIL_CHAINING_HPP
#define DOCWIRE_AUDIT_DETAIL_CHAINING_HPP

#include <type_traits>

namespace docwire::audit::detail
{

struct empty {};

template<typename Next>
using next_storage_t = std::conditional_t<std::is_void_v<Next>, empty, Next>;

} // namespace docwire::audit::detail

#endif
