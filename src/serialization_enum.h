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

#ifndef DOCWIRE_SERIALIZATION_ENUM_H
#define DOCWIRE_SERIALIZATION_ENUM_H

#include "serialization_base.h"
#include <magic_enum/magic_enum.hpp>

namespace docwire::serialization
{

template <typename T> requires std::is_enum_v<T>
struct serializer<T>
{
    value full(const T& value) const { return std::string{magic_enum::enum_name(value)}; }
    value typed_summary(const T& value) const { return decorate_with_typeid(full(value), type_name::pretty<T>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_ENUM_H
