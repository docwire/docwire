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

#ifndef DOCWIRE_SERIALIZATION_THREAD_ID_H
#define DOCWIRE_SERIALIZATION_THREAD_ID_H

#include "core_export.h"
#include "serialization_base.h"
#include <thread>

namespace docwire::serialization
{

template <>
struct serializer<std::thread::id>
{
    DOCWIRE_CORE_EXPORT value full(const std::thread::id& i) const;
    value typed_summary(const std::thread::id& i) const { return decorate_with_typeid(full(i), type_name::pretty<std::thread::id>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_THREAD_ID_H
