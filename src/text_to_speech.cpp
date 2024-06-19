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
#include <fstream>
#include "input.h"
#include "log.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{
namespace openai
{

struct TextToSpeech::Implementation
{
	std::string m_api_key;
	Model m_model;
	Voice m_voice;
};

TextToSpeech::TextToSpeech(const std::string& api_key, Model model, Voice voice)
	: impl(new Implementation{api_key, model, voice})
{
	docwire_log_func();
}

TextToSpeech::TextToSpeech(const TextToSpeech& other)
	: impl(new Implementation(*other.impl))
{
	docwire_log_func();
}

TextToSpeech::~TextToSpeech()
{
}

namespace
{

std::string model_to_string(TextToSpeech::Model model)
{
	switch (model)
	{
		case TextToSpeech::Model::tts1: return "tts-1";
		case TextToSpeech::Model::tts1_hd: return "tts-1-hd";
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
	catch (const http::Post::RequestFailed& e)
	{
		throw TextToSpeech::HttpError("Http POST failed: " + query, e);
	}
	return response_stream.str();
}

/*std::string parse_response(const std::string& response)
{
	docwire_log_func_with_args(response);
	try
	{
		boost::json::value response_val = boost::json::parse(response);
		return response_val.as_object()["choices"].as_array()[0].as_object()["message"].as_object()["content"].as_string().c_str();
	}
	catch (const std::exception& e)
	{
		throw Chat::ParseResponseError("Error parsing response: " + response, e);
	}
}*/

} // anonymous namespace

void TextToSpeech::process(Info &info) const
{
	docwire_log_func();
	if (!std::holds_alternative<data_source>(info.tag))
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "data_source received";
	const data_source& data = std::get<data_source>(info.tag);
	std::string data_str = data.string();
	std::string content = post_request(prepare_query(data_str, impl->m_model, impl->m_voice), impl->m_api_key);
	Info new_info(data_source{content});
	emit(new_info);
}

} // namespace openai
} // namespace docwire
