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

#ifndef DOCWIRE_AUDIT_JSON_STREAM_AUDITOR_HPP
#define DOCWIRE_AUDIT_JSON_STREAM_AUDITOR_HPP

#include "filter.hpp"
#include "serialize/json.hpp"
#include "sink/ostream.hpp"
#include "../ref_or_owned.h"
#include "../source_location.h"

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

namespace df0::audit
{

/**
 * @brief Predefined JSON audit pipeline: filter -> JSON serializer -> output stream.
 *
 * This named type is built from the composable audit components.
 * It is suitable for use as the configured auditor type in application code
 * and for explicit template instantiation in SDK translation units.
 */
class json_stream_auditor
{
public:
    json_stream_auditor(
        std::string filter_spec,
        ref_or_owned<std::ostream> stream)
        : m_impl(
            filter::by_spec{std::move(filter_spec)}
            | serialize::json{}
            | sink::ostream{std::move(stream)}
        )
    {
    }

    json_stream_auditor(const json_stream_auditor&) = delete;
    json_stream_auditor& operator=(const json_stream_auditor&) = delete;

    json_stream_auditor(json_stream_auditor&&) noexcept = default;
    json_stream_auditor& operator=(json_stream_auditor&&) noexcept = default;

    template<typename... Args>
    void record(source_location location, Args&&... args)
    {
        m_impl.record(location, std::forward<Args>(args)...);
    }

private:
    using impl_type =
        filter::by_spec<serialize::json<sink::ostream>>;

    impl_type m_impl;
};

} // namespace df0::audit

#endif
