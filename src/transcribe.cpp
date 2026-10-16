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

#include "transcribe.h"

#include <boost/json.hpp>
#include "document_elements.h"
#include "pipeline/input_element.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "make_error.h"
#include "pipeline/output_element.hpp"
#include "post.h"
#include "serialization_enum.h" // IWYU pragma: keep
#include <sstream>

namespace docwire
{

template<typename auditor_type>
struct pimpl_impl<openai::transcribe<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
	using audit::with_auditor<auditor_type>::auditor;

	explicit pimpl_impl(
		const std::string& api_key,
		typename openai::transcribe<auditor_type>::model model,
		audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>{auditor},
		  m_api_key(api_key),
		  m_model(model)
	{}

	std::string m_api_key;
	typename openai::transcribe<auditor_type>::model m_model;
};

namespace openai
{

template<typename auditor_type>
transcribe<auditor_type>::transcribe(
	const std::string& api_key,
	model model,
	audit::auditor_ref<auditor_type> auditor)
	: with_pimpl<transcribe<auditor_type>>(api_key, model, auditor),
	  audit::with_auditor<auditor_type>(auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{ return "model"_v = model; }};
}

namespace
{

template<typename auditor_type>
std::string model_to_string(typename openai::transcribe<auditor_type>::model model)
{
	switch (model)
	{
		case openai::transcribe<auditor_type>::model::gpt_4o_transcribe: return "gpt-4o-transcribe";
		case openai::transcribe<auditor_type>::model::gpt_4o_mini_transcribe: return "gpt-4o-mini-transcribe";
		case openai::transcribe<auditor_type>::model::whisper_1: return "whisper-1";
		default: return "?";
	}
}

} // anonymous namespace

template<typename auditor_type>
continuation transcribe<auditor_type>::operator()(
	message_ptr msg,
	const message_callbacks& emit_message)
{
	audit::scope audit_scope{this->auditor(), [&]{ return "msg"_v = msg; }};
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	const data_source& data = msg->get<data_source>();
	std::shared_ptr<std::istream> in_stream = data.istream();
	auto response_stream = std::make_shared<std::ostringstream>();
	try
	{
		in_stream | http::post<>("https://api.openai.com/v1/audio/transcriptions", {{"model", model_to_string<auditor_type>(impl().m_model)}, {"response_format", "text"}}, "file", default_file_name("audio.mp3"), impl().m_api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error during transcription"));
	}
    auto cont = emit_message(document::document{});
    if (cont == continuation::stop) return cont;
    cont = emit_message(document::text{response_stream->str()});
    if (cont == continuation::stop) return cont;
    return emit_message(document::close_document{});
}

template class DOCWIRE_OPENAI_EXPORT docwire::openai::transcribe<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::transcribe<docwire::audit::legacy_log_audit>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::transcribe<docwire::audit::json_stream_auditor>;

} // namespace openai
} // namespace docwire
