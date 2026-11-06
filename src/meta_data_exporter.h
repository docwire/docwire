/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_META_DATA_EXPORTER_H
#define DF0_META_DATA_EXPORTER_H

#include "pipeline/element_base.hpp"
#include "core_export.h"
#include <concepts>
#include <memory>
#include <sstream>
#include "document_elements.h"
#include "meta_data_writer.h"
#include "audit.hpp"
#include "data_source.h"
#include "serialization_message.h"

namespace df0
{

/**
 * @brief Exports meta data only to plain text format.
 */
template<typename auditor_type = audit::null_auditor>
class metadata_exporter : public pipeline::transformer_element<metadata_exporter<auditor_type>>, private audit::with_auditor<auditor_type>
{
    using audit::with_auditor<auditor_type>::auditor;
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
  metadata_exporter()
    : metadata_exporter(audit::auditor_ref<stateless_auditor_type>{})
  {}

  explicit metadata_exporter(audit::auditor_ref<auditor_type> auditor)
    : audit::with_auditor<auditor_type>(auditor)
  {}

	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

private:
  std::shared_ptr<std::stringstream> m_stream;
  metadata_writer m_writer;
};

template<typename auditor_type>
inline continuation metadata_exporter<auditor_type>::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	audit::scope audit_scope{auditor(), [&]{ return "msg"_v = msg; }};
	if (msg->is<std::exception_ptr>())
		return emit_message(std::move(msg));
	if (msg->is<document::document>() || !m_stream)
		m_stream = std::make_shared<std::stringstream>();
	m_writer.write_to(msg, *m_stream);
	if (msg->is<document::close_document>())
	{
		emit_message(data_source{seekable_stream_ptr{m_stream}});
		m_stream.reset();
	}
	return continuation::proceed;
}

// Deduction guide for CTAD
metadata_exporter() -> metadata_exporter<audit::null_auditor>;

} // namespace df0

#endif //DF0_META_DATA_EXPORTER_H
