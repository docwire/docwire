/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "text_to_speech.h"

#include <boost/json.hpp>
#include "input.h"
#include "log.h"
#include "make_error.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{

using namespace openai;

template<>
struct pimpl_impl<openai::TextToSpeech> : pimpl_impl_base
{
	pimpl_impl(const std::string& api_key, TextToSpeech::Model model, TextToSpeech::Voice voice)
		: m_api_key(api_key), m_model(model), m_voice(voice) {}
	std::string m_api_key;
	TextToSpeech::Model m_model;
	TextToSpeech::Voice m_voice;
};

namespace openai
{

TextToSpeech::TextToSpeech(const std::string& api_key, Model model, Voice voice)
	: with_pimpl<TextToSpeech>(api_key, model, voice)
{
	docwire_log_func();
}

namespace
{

std::string model_to_string(TextToSpeech::Model model)
{
	switch (model)
	{
		case TextToSpeech::Model::gpt_4o_mini_tts: return "gpt-4o-mini-tts";
		case TextToSpeech::Model::tts_1: return "tts-1";
		case TextToSpeech::Model::tts_1_hd: return "tts-1-hd";
		default: return "?";
	}
}

std::string voice_to_string(TextToSpeech::Voice voice)
{
	switch (voice)
	{
		case TextToSpeech::Voice::alloy: return "alloy";
		case TextToSpeech::Voice::echo: return "echo";
		case TextToSpeech::Voice::fable: return "fable";
		case TextToSpeech::Voice::onyx: return "onyx";
		case TextToSpeech::Voice::nova: return "nova";
		case TextToSpeech::Voice::shimmer: return "shimmer";
		default: return "?";
	}
}

std::string prepare_query(const std::string& input, TextToSpeech::Model model, TextToSpeech::Voice voice)
{
	docwire_log_func();
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
	docwire_log_func_with_args(query);
	std::ostringstream response_stream{};
	try
	{
		std::stringstream{ query } | http::Post("https://api.openai.com/v1/audio/speech", api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

} // anonymous namespace

continuation TextToSpeech::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
	docwire_log_func();
	if (!std::holds_alternative<data_source>(tag))
		return emit_tag(std::move(tag));
	docwire_log(debug) << "data_source received";
	const data_source& data = std::get<data_source>(tag);
	std::string data_str = data.string();
	std::string content = post_request(prepare_query(data_str, impl().m_model, impl().m_voice), impl().m_api_key);
	return emit_tag(data_source{content});
}

} // namespace openai
} // namespace docwire
