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
struct serializer<mail::Attachment>
{
    value full(const mail::Attachment& attachment) const
    {
        return object{{
            {"name", serialization::full(attachment.name)},
            {"size", static_cast<int64_t>(attachment.size)},
            {"extension", serialization::full(attachment.extension)}
        }};
    }
    value typed_summary(const mail::Attachment& attachment) const { return decorate_with_typeid(full(attachment), type_name::pretty<mail::Attachment>()); }
};

template<>
struct serializer<mail::Mail>
{
    value full(const mail::Mail& mail) const
    {
        return object{{
            {"subject", serialization::full(mail.subject)},
            {"date", serialization::full(mail.date)},
            {"level", serialization::full(mail.level)}
        }};
    }
    value typed_summary(const mail::Mail& mail) const { return decorate_with_typeid(full(mail), type_name::pretty<mail::Mail>()); }
};

template<>
struct serializer<mail::Folder>
{
    value full(const mail::Folder& folder) const
    {
        return object{{
            {"name", serialization::full(folder.name)},
            {"level", serialization::full(folder.level)}
        }};
    }
    value typed_summary(const mail::Folder& folder) const { return decorate_with_typeid(full(folder), type_name::pretty<mail::Folder>()); }
};

}

#endif // DOCWIRE_SERIALIZATION_MAIL_ELEMENTS_H
