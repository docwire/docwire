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

#ifndef DOCWIRE_SERIALIZATION_TYPEINDEX_H
#define DOCWIRE_SERIALIZATION_TYPEINDEX_H

#include "core_export.h"
#include "serialization_base.h"
#include <typeindex>

namespace docwire::serialization
{

template <>
struct serializer<std::type_index>
{
    DOCWIRE_CORE_EXPORT value full(const std::type_index& t) const;
    value typed_summary(const std::type_index& t) const { return decorate_with_typeid(this->full(t), type_name::pretty<std::type_index>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_TYPEINDEX_H
