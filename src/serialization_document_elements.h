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

#ifndef DOCWIRE_SERIALIZATION_DOCUMENT_ELEMENTS_H
#define DOCWIRE_SERIALIZATION_DOCUMENT_ELEMENTS_H

#include "attributes.h"
#include "document_elements.h"
#include "serialization_base.h"
#include "serialization_filesystem.h" // IWYU pragma: keep

namespace docwire::serialization
{

/**
 * @brief Concept for types that have a `styling` member.
 */
template <typename T>
concept with_styling = requires(T t) { t.styling; };

/**
 * @brief Generic serializer for types with a `styling` member.
 */
template <with_styling T>
struct serializer<T> {
    value full(const T& val) const { return object{{{"styling", full(val.styling)}}}; }
    value typed_summary(const T& val) const { return decorate_with_typeid(full(val), type_name::pretty<T>()); }
};

template <> struct serializer<document::text>
{
    value full(const document::text& text) const
    {
        return object{{{"text", text.text}}};
    }
    value typed_summary(const document::text& text) const { return decorate_with_typeid(full(text), type_name::pretty<document::text>()); }
};

template <> struct serializer<document::link>
{
    value full(const document::link& link) const
    {
        return object{{
            {"url", serialization::full(link.url)},
            {"styling", serialization::full(link.styling)}
        }};
    }
    value typed_summary(const document::link& link) const { return decorate_with_typeid(full(link), type_name::pretty<document::link>()); }
};

template <> struct serializer<document::image>
{
    value full(const document::image& image) const
    {
        return object{{
            {"alt", serialization::full(image.alt)},
            {"styling", serialization::full(image.styling)}
        }};
    }
    value typed_summary(const document::image& image) const { return decorate_with_typeid(serialization::full(image), type_name::pretty<document::image>()); }
};

template <> struct serializer<document::style>
{
    value full(const document::style& style) const
    {
        return object{{{"css_text", style.css_text}}};
    }
    value typed_summary(const document::style& style) const { return decorate_with_typeid(serialization::full(style), type_name::pretty<document::style>()); }
};

template <> struct serializer<document::list>
{
    value full(const document::list& list) const
    {
        return object{{
            {"type", serialization::full(list.type)},
            {"styling", serialization::full(list.styling)}
        }};
    }
    value typed_summary(const document::list& list) const { return decorate_with_typeid(serialization::full(list), type_name::pretty<document::list>()); }
};

template <> struct serializer<document::comment>
{
    value full(const document::comment& comment) const
    {
        return object{{
            {"author", serialization::full(comment.author)},
            {"time", serialization::full(comment.time)},
            {"comment", serialization::full(comment.comment)}
        }};
    }
    value typed_summary(const document::comment& comment) const { return decorate_with_typeid(full(comment), type_name::pretty<document::comment>()); }
};

template <>
struct serializer<attributes::styling>
{
    value full(const attributes::styling& styling) const
    {
        return object{{
            {"classes", serialization::full(styling.classes)},
            {"id", serialization::full(styling.id)},
            {"style", serialization::full(styling.style)}
        }};
    }
    value typed_summary(const attributes::styling& styling) const { return decorate_with_typeid(serialization::full(styling), type_name::pretty<attributes::styling>()); }
};

} // namespace docwire::serialization

#endif // DOCWIRE_SERIALIZATION_DOCUMENT_ELEMENTS_H