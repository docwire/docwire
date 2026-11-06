/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "transcribe.h"

#include <boost/json.hpp>
#include "document_elements.h"
#include "pipeline/input_element.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "make_error.h"
#include "pipeline/output_element.hpp"
#include "post.h"
#include "serialization_enum.h" // IWYU pragma: keep
#include <map>
#include <sstream>

namespace df0
{

template<typename auditor_type>
struct pimpl_impl<openai::transcribe<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
	using audit::with_auditor<auditor_type>::auditor;

	explicit pimpl_impl(
		const std::string& api_key,
		openai::transcribe_model model,
		audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>{auditor},
		  m_api_key(api_key),
		  m_model(model)
	{}

	std::string m_api_key;
	openai::transcribe_model m_model;
};

namespace openai
{

template<typename auditor_type>
transcribe<auditor_type>::transcribe(
	const std::string& api_key,
	transcribe_model model,
	audit::auditor_ref<auditor_type> auditor)
	: with_pimpl<transcribe<auditor_type>>(api_key, model, auditor),
	  audit::with_auditor<auditor_type>(auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{ return "model"_v = model; }};
}

namespace
{

std::string model_to_string(openai::transcribe_model model)
{
	switch (model)
	{
		case openai::transcribe_model::gpt_4o_transcribe: return "gpt-4o-transcribe";
		case openai::transcribe_model::gpt_4o_mini_transcribe: return "gpt-4o-mini-transcribe";
		case openai::transcribe_model::whisper_1: return "whisper-1";
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
		auto auditor_ref = this->auditor();

		in_stream | http::post<auditor_type>(
			"https://api.openai.com/v1/audio/transcriptions",
			std::map<std::string, std::string>{
				{"model", model_to_string(impl().m_model)},
				{"response_format", "text"}
			},
			"file",
			default_file_name("audio.mp3"),
			impl().m_api_key,
			http::ssl_verify_peer{true},
			auditor_ref
		) | response_stream;
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

template class DF0_OPENAI_EXPORT df0::openai::transcribe<df0::audit::null_auditor>;
template class DF0_OPENAI_EXPORT df0::openai::transcribe<df0::audit::json_stream_auditor>;

} // namespace openai
} // namespace df0
