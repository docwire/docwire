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
#include "input.h"
#include "log_entry.h"
#include "log_scope.h"
#include "make_error.h"
#include "output.h"
#include "post.h"
#include "serialization_enum.h" // IWYU pragma: keep
#include <sstream>

namespace docwire
{

using namespace openai;

template<>
struct pimpl_impl<openai::text_to_speech> : pimpl_impl_base
{
	pimpl_impl(const std::string& api_key, text_to_speech::Model model, text_to_speech::Voice voice)
		: m_api_key(api_key), m_model(model), m_voice(voice) {}
	std::string m_api_key;
	text_to_speech::Model m_model;
	text_to_speech::Voice m_voice;
};

namespace openai
{

text_to_speech::text_to_speech(const std::string& api_key, Model model, Voice voice)
	: with_pimpl<text_to_speech>(api_key, model, voice)
{
	log_scope(model, voice);
}

namespace
{

std::string model_to_string(text_to_speech::Model model)
{
	switch (model)
	{
		case text_to_speech::Model::gpt_4o_mini_tts: return "gpt-4o-mini-tts";
		case text_to_speech::Model::tts_1: return "tts-1";
		case text_to_speech::Model::tts_1_hd: return "tts-1-hd";
		default: return "?";
	}
}

std::string voice_to_string(text_to_speech::Voice voice)
{
	switch (voice)
	{
		case text_to_speech::Voice::alloy: return "alloy";
		case text_to_speech::Voice::echo: return "echo";
		case text_to_speech::Voice::fable: return "fable";
		case text_to_speech::Voice::onyx: return "onyx";
		case text_to_speech::Voice::nova: return "nova";
		case text_to_speech::Voice::shimmer: return "shimmer";
		default: return "?";
	}
}

std::string prepare_query(const std::string& input, text_to_speech::Model model, text_to_speech::Voice voice)
{
	log_scope(input, model, voice);
	boost::json::object query
	{
		{ "model", model_to_string(model) },
		{ "input", input },
		{ "voice", voice_to_string(voice) }
	};
	return boost::json::serialize(query);
}

std::string post_request(const std::string& query, const std::string& api_key)
{
	log_scope(query);
	std::ostringstream response_stream{};
	try
	{
		data_source{query, mime_type{"application/json"}, confidence::highest} |
			http::post("https://api.openai.com/v1/audio/speech", api_key) |
			response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

} // anonymous namespace

continuation text_to_speech::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log_scope();
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	log_entry();
	const data_source& data = msg->get<data_source>();
	std::string data_str = data.string();
	std::string content = post_request(prepare_query(data_str, impl().m_model, impl().m_voice), impl().m_api_key);
	return emit_message(data_source{content});
}

} // namespace openai
} // namespace docwire
