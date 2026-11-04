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

#ifndef DOCWIRE_MAIL_PARSER_H
#define DOCWIRE_MAIL_PARSER_H

#include "eml_parser.h"
#include "pipeline/composition.hpp"
#include "pst_parser.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

#include <concepts>
#include <utility>

namespace df0
{

// Forward declaration so the detail alias can reference the class.
template<typename auditor_type>
class mail_parser;

namespace detail
{

template<typename auditor_type>
using mail_parser_base = pipeline::composition_wrapper<
    mail_parser<auditor_type>,
    eml_parser<auditor_type>,
    pst_parser<auditor_type>>;

} // namespace detail

/**
 * @brief Composite parser for email formats.
 *
 * This is a named composition of the individual email parsers. It can be
 * constructed with either:
 *
 * - `mail_parser{}` for the default no-op auditor, or
 * - `mail_parser<auditor_type>{auditor}` for an explicit auditor.
 *
 * @tparam auditor_type Injected auditor type.
 */
template<typename auditor_type = audit::null_auditor>
class mail_parser
    : public detail::mail_parser_base<auditor_type>
{
public:
    using base_type = detail::mail_parser_base<auditor_type>;

    /**
     * @brief Constructs the composite parser with the default stateless audit policy.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    mail_parser()
        : mail_parser(audit::auditor_ref<stateless_auditor_type>{})
    {}

    /**
     * @brief Constructs the composite parser with an explicit injected auditor.
     */
    explicit mail_parser(audit::auditor_ref<auditor_type> auditor)
        : base_type(auditor)
    {}
};

// Default deduction guide enabling `mail_parser{}`.
mail_parser() -> mail_parser<audit::null_auditor>;

} // namespace df0

#endif //DOCWIRE_MAIL_PARSER_H
