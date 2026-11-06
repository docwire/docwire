/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_HTML_EXPORTER_H
#define DF0_HTML_EXPORTER_H

#include "pipeline/element_base.hpp"
#include "html_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>

namespace df0
{

/**
 * @brief Exports data to HTML format.
 */
template<typename auditor_type = audit::null_auditor>
class DF0_HTML_EXPORT html_exporter
    : public pipeline::transformer_element<html_exporter<auditor_type>>,
      public with_pimpl<html_exporter<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
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
  html_exporter()
    : html_exporter(audit::auditor_ref<stateless_auditor_type>{})
  {}

  explicit html_exporter(audit::auditor_ref<auditor_type> auditor);

  continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
  using with_pimpl<html_exporter<auditor_type>>::impl;
  using audit::with_auditor<auditor_type>::auditor;

  friend pimpl_impl<html_exporter<auditor_type>>;
};

// Deduction guide for CTAD
html_exporter() -> html_exporter<audit::null_auditor>;

} // namespace df0

#endif //DF0_HTML_EXPORTER_H
