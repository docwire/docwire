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

#ifndef DOCWIRE_SERIALIZATION_MESSAGE_H
#define DOCWIRE_SERIALIZATION_MESSAGE_H

#include "message.h"
#include "serialization_base.h"

namespace docwire::serialization
{

template <>
struct serializer<message_base>
{
    value full(const message_base& msg) const
    {
        return object{{{"object_type", type_name::from_type_index(msg.object_type())}}};
    }
    value typed_summary(const message_base& msg) const { return decorate_with_typeid(full(msg), type_name::pretty<message_base>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_MESSAGE_H
