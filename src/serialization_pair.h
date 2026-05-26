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

#ifndef DOCWIRE_SERIALIZATION_PAIR_H
#define DOCWIRE_SERIALIZATION_PAIR_H

#include "serialization_base.h"
#include <utility>

namespace docwire::serialization
{

template <typename T1, typename T2>
struct serializer<std::pair<T1, T2>>
{
    value full(const std::pair<T1, T2>& pair) const
    {
        return object{{
            {"first", serialization::full(pair.first)},
            {"second", serialization::full(pair.second)}
        }};
    }

    value typed_summary(const std::pair<T1, T2>& pair) const
    {
        return decorate_with_typeid(object{{
            {"first", serialization::typed_summary(pair.first)},
            {"second", serialization::typed_summary(pair.second)}
        }}, type_name::pretty<std::pair<T1, T2>>());
    }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_PAIR_H
