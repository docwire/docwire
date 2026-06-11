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

#ifndef DOCWIRE_SERIALIZATION_DATA_SOURCE_H
#define DOCWIRE_SERIALIZATION_DATA_SOURCE_H

#include "data_source.h"
#include "serialization_base.h"
#include "serialization_file_extension.h" // IWYU pragma: keep
#include "serialization_filesystem.h" // IWYU pragma: keep

namespace docwire::serialization
{

template <>
struct serializer<data_source>
{
    value full(const data_source& data) const
    {
        return object{{
            {"path", serialization::full(data.path())},
            {"file_extension", serialization::full(data.file_extension())}
        }};
    }

    value typed_summary(const data_source& data) const
    {
        return decorate_with_typeid(object{{
            {"path", serialization::typed_summary(data.path())},
            {"file_extension", serialization::typed_summary(data.file_extension())}
        }}, type_name::pretty<data_source>());
    }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_DATA_SOURCE_H
