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

#ifndef DOCWIRE_LOG_VARIANT_H
#define DOCWIRE_LOG_VARIANT_H

#include "log.h"
#include <variant>

namespace docwire
{

template<typename... Ts>
log_record_stream& operator<<(log_record_stream& log_stream, const std::variant<Ts...>& variant)
{
    std::visit([&](const auto& value)
    {
        log_stream << begin_complex() << docwire_log_streamable_type_of(variant) << std::make_pair("value", value) << end_complex();
    }, variant);
    return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_VARIANT_H
