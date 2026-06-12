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

#ifndef DOCWIRE_SERIALIZATION_MAIL_ELEMENTS_H
#define DOCWIRE_SERIALIZATION_MAIL_ELEMENTS_H

#include "mail_elements.h"
#include "serialization_base.h"
#include "serialization_file_extension.h" // IWYU pragma: keep

namespace docwire::serialization
{

template<>
struct serializer<mail::attachment>
{
    value full(const mail::attachment& attachment) const
    {
        return object{{
            {"name", serialization::full(attachment.name)},
            {"size", static_cast<int64_t>(attachment.size)},
            {"extension", serialization::full(attachment.extension)}
        }};
    }
    value typed_summary(const mail::attachment& attachment) const { return decorate_with_typeid(full(attachment), type_name::pretty<mail::attachment>()); }
};

template<>
struct serializer<mail::mail>
{
    value full(const mail::mail& mail) const
    {
        return object{{
            {"subject", serialization::full(mail.subject)},
            {"date", serialization::full(mail.date)},
            {"level", serialization::full(mail.level)}
        }};
    }
    value typed_summary(const mail::mail& mail) const { return decorate_with_typeid(full(mail), type_name::pretty<mail::mail>()); }
};

template<>
struct serializer<mail::folder>
{
    value full(const mail::folder& folder) const
    {
        return object{{
            {"name", serialization::full(folder.name)},
            {"level", serialization::full(folder.level)}
        }};
    }
    value typed_summary(const mail::folder& folder) const { return decorate_with_typeid(full(folder), type_name::pretty<mail::folder>()); }
};

}

#endif // DOCWIRE_SERIALIZATION_MAIL_ELEMENTS_H
