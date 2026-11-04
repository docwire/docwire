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

#ifndef DOCWIRE_AUDIT_SERIALIZE_JSON_HPP
#define DOCWIRE_AUDIT_SERIALIZE_JSON_HPP

#include "../detail/chaining.hpp"
#include "../detail/value_conversion.hpp"
#include "../../serialization_base.h"
#include "../../source_location.h"

#include <type_traits>
#include <utility>

namespace df0::audit::serialize
{

template<typename Next = void>
class json
{
    template<typename> friend class json;

public:
    json() requires std::is_void_v<Next> = default;

    template<typename R>
    auto operator|(R&& next) &&
    {
        return std::move(*this).with_next(std::forward<R>(next));
    }

    template<typename R>
    auto with_next(R&& next) &&
    {
        return json<std::decay_t<R>>(std::forward<R>(next));
    }

    template<typename... Args>
        requires (!std::is_void_v<Next>)
    void record(source_location location, Args&&... args)
    {
        auto context = detail::build_context(std::forward<Args>(args)...);
        auto record_object = detail::create_record_object(location, std::move(context));
        m_next.record(location, std::move(record_object));
    }

private:
    template<typename NextParam>
    explicit json(NextParam&& next)
        requires (!std::is_void_v<Next> &&
                  std::constructible_from<detail::next_storage_t<Next>, NextParam&&>)
        : m_next(std::forward<NextParam>(next))
    {
    }

    detail::next_storage_t<Next> m_next;
};

} // namespace df0::audit::serialize

#endif
