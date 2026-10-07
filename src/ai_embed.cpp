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

#include "ai_embed.h"
#include "ai_elements.h"
#include "data_source.h"
#include "error_tags.h"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "serialization_message.h" // IWYU pragma: keep
#include <string>
#include "throw_if.h"
#include <vector>

namespace docwire
{

template<typename auditor_type>
struct pimpl_impl<ai::embed<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
    using audit::with_auditor<auditor_type>::auditor;

    not_null<std::shared_ptr<ai::ai_runner>> m_model_runner;
    std::string m_prefix;

    pimpl_impl(
        not_null<std::shared_ptr<ai::ai_runner>> model_runner,
        std::string prefix,
        audit::auditor_ref<auditor_type> auditor)
        : audit::with_auditor<auditor_type>{auditor},
          m_model_runner(std::move(model_runner)),
          m_prefix(std::move(prefix))
    {
    }
};

namespace ai
{

template<typename auditor_type>
embed<auditor_type>::embed(
    not_null<std::shared_ptr<ai_runner>> model_runner,
    std::string prefix,
    audit::auditor_ref<auditor_type> auditor)
    : with_pimpl<embed<auditor_type>>(
          std::move(model_runner), std::move(prefix), auditor),
      audit::with_auditor<auditor_type>(auditor)
{}

template<typename auditor_type>
continuation embed<auditor_type>::operator()(
    message_ptr msg,
    const message_callbacks& emit_message)
{
    audit::scope audit_scope{this->auditor(), [&]{ return "msg"_v = msg; }};
    if (!msg->is<data_source>())
        return emit_message(std::move(msg));

    const data_source& data = msg->get<data_source>();
    throw_if(!data.has_highest_confidence_mime_type_in({mime_type{"text/plain"}}), "Input for ai::embed must be text/plain", errors::program_logic{});
    std::string data_str = data.string();

    std::string prefixed_input = impl().m_prefix + data_str;
    std::vector<double> embedding_vector = impl().m_model_runner->embed(prefixed_input);
    return emit_message(ai::embedding{std::move(embedding_vector)});
}
template class DOCWIRE_AI_EXPORT docwire::ai::embed<docwire::audit::null_auditor>;
template class DOCWIRE_AI_EXPORT docwire::ai::embed<docwire::audit::legacy_log_audit>;

} // namespace ai
} // namespace docwire
