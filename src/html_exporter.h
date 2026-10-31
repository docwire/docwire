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

#ifndef DOCWIRE_HTML_EXPORTER_H
#define DOCWIRE_HTML_EXPORTER_H

#include "pipeline/element_base.hpp"
#include "html_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"

namespace docwire
{

/**
 * @brief Exports data to HTML format.
 */
template<typename auditor_type = audit::null_auditor>
class DOCWIRE_HTML_EXPORT html_exporter
    : public pipeline::transformer_element<html_exporter<auditor_type>>,
      public with_pimpl<html_exporter<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
  explicit html_exporter(audit::auditor_ref<auditor_type> auditor = {});

  continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
  using with_pimpl<html_exporter<auditor_type>>::impl;
  using audit::with_auditor<auditor_type>::auditor;

  friend pimpl_impl<html_exporter<auditor_type>>;
};

} // namespace docwire

#endif //DOCWIRE_HTML_EXPORTER_H
