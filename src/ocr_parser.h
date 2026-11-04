/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_OCR_PARSER_H
#define DF0_OCR_PARSER_H

#include "pipeline/element_base.hpp"
#include <cstdint>
#include "data_source.h"
#include <filesystem>
#include "language.h"
#include "ocr_export.h"
#include <concepts>
#include <optional>
#include "pimpl.h"
#include <vector>
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace df0
{

namespace ocr
{

struct please_wait {};

} // namespace ocr

struct ocr_confidence_threshold { std::optional<float> v; };
struct ocr_data_path { std::filesystem::path v; };
struct ocr_timeout { std::optional<int32_t> v; };

template<typename auditor_type = audit::null_auditor>
class DF0_OCR_EXPORT ocr_parser
    : public pipeline::transformer_element<ocr_parser<auditor_type>>,
      public with_pimpl<ocr_parser<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
private:
    using with_pimpl<ocr_parser<auditor_type>>::impl;
    using audit::with_auditor<auditor_type>::auditor;
    friend pimpl_impl<ocr_parser<auditor_type>>;

public:

    /**
     * @brief Constructs the component with the default stateless auditing policy.
     *
     * This overload is available only when the configured auditor is stateless
     * and therefore default-constructible.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    ocr_parser()
        : ocr_parser(
              {},
              {},
              {},
              {},
              audit::auditor_ref<stateless_auditor_type>{})
    {}

    /**
     * @brief Constructs the component with only an OCR language list.
     *
     * Other OCR options use their defaults, and the default stateless audit policy
     * is selected automatically.
     *
     * @tparam stateless_auditor_type Injected auditor type selected by this overload.
     */
    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    ocr_parser(const std::vector<language>& languages)
        : ocr_parser(
              languages,
              {},
              {},
              {},
              audit::auditor_ref<stateless_auditor_type>{})
    {}

    template<audit::stateless_auditor stateless_auditor_type = auditor_type>
    ocr_parser(
        const std::vector<language>& languages,
        ocr_confidence_threshold ocr_confidence_threshold_arg,
        ocr_timeout ocr_timeout_arg,
        ocr_data_path ocr_data_path_arg)
        : ocr_parser(
              languages,
              ocr_confidence_threshold_arg,
              ocr_timeout_arg,
              ocr_data_path_arg,
              audit::auditor_ref<stateless_auditor_type>{})
    {}

    explicit ocr_parser(const std::vector<language>& languages,
        ocr_confidence_threshold ocr_confidence_threshold_arg,
        ocr_timeout ocr_timeout_arg,
        ocr_data_path ocr_data_path_arg,
        audit::auditor_ref<auditor_type> auditor);

    continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
    void parse(const data_source& data, const std::vector<language>& languages);
};

// Deduction guides for CTAD
ocr_parser() -> ocr_parser<audit::null_auditor>;
ocr_parser(const std::vector<language>&) -> ocr_parser<audit::null_auditor>;
ocr_parser(
    const std::vector<language>&,
    ocr_confidence_threshold,
    ocr_timeout,
    ocr_data_path) -> ocr_parser<audit::null_auditor>;

} // namespace df0

#endif // DF0_OCR_PARSER_H
