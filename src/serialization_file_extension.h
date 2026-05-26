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

#ifndef DOCWIRE_SERIALIZATION_FILE_EXTENSION_H
#define DOCWIRE_SERIALIZATION_FILE_EXTENSION_H

#include "file_extension.h"
#include "serialization_base.h"

namespace docwire::serialization
{

/**
 * @brief Specialization for `file_extension`.
 */
template <>
struct serializer<file_extension>
{
    value full(const file_extension& ext) const
    {
        return object{{{"value", ext.string()}}};
    }
    value typed_summary(const file_extension& ext) const { return decorate_with_typeid(full(ext), type_name::pretty<file_extension>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_FILE_EXTENSION_H
