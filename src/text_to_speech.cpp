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

#include "text_to_speech.h"

#include <boost/json.hpp>
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

using namespace openai;

template<typename auditor_type>
struct pimpl_impl<openai::text_to_speech<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
	using audit::with_auditor<auditor_type>::auditor;

	explicit pimpl_impl(
		const std::string& api_key,
		openai::text_to_speech_model model,
		openai::text_to_speech_voice voice,
		audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>{auditor},
		  m_api_key(api_key),
		  m_model(model),
		  m_voice(voice)
	{}

	std::string m_api_key;
	openai::text_to_speech_model m_model;
	openai::text_to_speech_voice m_voice;
};

namespace openai
{

template<typename auditor_type>
text_to_speech<auditor_type>::text_to_speech(
	const std::string& api_key,
	text_to_speech_model model,
	text_to_speech_voice voice,
	audit::auditor_ref<auditor_type> auditor)
	: with_pimpl<text_to_speech<auditor_type>>(api_key, model, voice, auditor),
	  audit::with_auditor<auditor_type>(auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{
		return "model"_v = model, "voice"_v = voice;
	}};
}

namespace
{

std::string model_to_string(openai::text_to_speech_model model)
{
	switch (model)
	{
		case openai::text_to_speech_model::gpt_4o_mini_tts: return "gpt-4o-mini-tts";
		case openai::text_to_speech_model::tts_1: return "tts-1";
		case openai::text_to_speech_model::tts_1_hd: return "tts-1-hd";
		default: return "?";
	}
}

std::string voice_to_string(openai::text_to_speech_voice voice)
{
	switch (voice)
	{
		case openai::text_to_speech_voice::alloy: return "alloy";
		case openai::text_to_speech_voice::echo: return "echo";
		case openai::text_to_speech_voice::fable: return "fable";
		case openai::text_to_speech_voice::onyx: return "onyx";
		case openai::text_to_speech_voice::nova: return "nova";
		case openai::text_to_speech_voice::shimmer: return "shimmer";
		default: return "?";
	}
}

template<typename auditor_type>
std::string prepare_query(
	const std::string& input,
	openai::text_to_speech_model model,
	openai::text_to_speech_voice voice,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{
		return "input"_v = input, "model"_v = model, "voice"_v = voice;
	}};
	boost::json::object query
	{
		{ "model", model_to_string(model) },
		{ "input", input },
		{ "voice", voice_to_string(voice) }
	};
	return boost::json::serialize(query);
}

template<typename auditor_type>
std::string post_request(
	const std::string& query,
	const std::string& api_key,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "query"_v = query; }};
	std::ostringstream response_stream{};
	try
	{
		data_source{query, mime_type{"application/json"}, confidence::highest} |
			http::post<auditor_type>(
				"https://api.openai.com/v1/audio/speech",
				api_key,
				http::ssl_verify_peer{true},
				auditor) |
			response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

} // anonymous namespace

template<typename auditor_type>
continuation text_to_speech<auditor_type>::operator()(
	message_ptr msg,
	const message_callbacks& emit_message)
{
	audit::scope audit_scope{this->auditor(), [&]{ return "msg"_v = msg; }};
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	const data_source& data = msg->get<data_source>();
	std::string data_str = data.string();
	auto auditor_ref = audit::auditor_ref<auditor_type>(this->auditor());
	std::string content =
		post_request<auditor_type>(
			prepare_query<auditor_type>(data_str, impl().m_model, impl().m_voice, auditor_ref),
			impl().m_api_key,
			auditor_ref);
	return emit_message(data_source{content});
}

template class DOCWIRE_OPENAI_EXPORT docwire::openai::text_to_speech<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::text_to_speech<docwire::audit::legacy_log_audit>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::text_to_speech<docwire::audit::json_stream_auditor>;

} // namespace openai
} // namespace docwire
