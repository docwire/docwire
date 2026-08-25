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

#ifndef DOCWIRE_ICONV_WRAPPER_H
#define DOCWIRE_ICONV_WRAPPER_H

#include <cstdint>
#include <string>
#include <string_view>
#include "core_export.h"

namespace docwire::detail::iconv_wrapper
{

struct state
{
    std::uintptr_t descriptor{}; // opaque iconv_t handle
    std::string from;
    std::string to;
};

DOCWIRE_CORE_EXPORT void open(state& st);
DOCWIRE_CORE_EXPORT void close(state& st) noexcept;
DOCWIRE_CORE_EXPORT std::string convert(state& st, std::string_view input);

} // namespace docwire::detail::iconv_wrapper

#endif
