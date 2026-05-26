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

#ifndef DOCWIRE_SERIALIZATION_FILESYSTEM_H
#define DOCWIRE_SERIALIZATION_FILESYSTEM_H

#include <filesystem>
#include "serialization_base.h"

namespace docwire::serialization
{

template <>
struct serializer<std::filesystem::path>
{
    value full(const std::filesystem::path& p) const
    {
        return p.string();
    }
    value typed_summary(const std::filesystem::path& p) const { return decorate_with_typeid(full(p), type_name::pretty<std::filesystem::path>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_FILESYSTEM_H
