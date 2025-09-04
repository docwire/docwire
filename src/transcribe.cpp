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

#include "transcribe.h"

#include <boost/json.hpp>
#include "document_elements.h"
#include "input.h"
#include "log.h"
#include "make_error.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{

template<>
struct pimpl_impl<openai::Transcribe> : pimpl_impl_base
{
	pimpl_impl(const std::string& api_key, openai::Transcribe::Model model)
		: m_api_key(api_key), m_model(model) {}
	std::string m_api_key;
	openai::Transcribe::Model m_model;
};

namespace openai
{

Transcribe::Transcribe(const std::string& api_key, Model model)
	: with_pimpl<Transcribe>(api_key, model)
{
	docwire_log_func();
}

namespace
{

std::string model_to_string(Transcribe::Model model)
{
	switch (model)
	{
		case Transcribe::Model::gpt_4o_transcribe: return "gpt-4o-transcribe";
		case Transcribe::Model::gpt_4o_mini_transcribe: return "gpt-4o-mini-transcribe";
		case Transcribe::Model::whisper_1: return "whisper-1";
		default: return "?";
	}
}

} // anonymous namespace

continuation Transcribe::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	docwire_log_func();
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	docwire_log(debug) << "data_source received";
	const data_source& data = msg->get<data_source>();
	std::shared_ptr<std::istream> in_stream = data.istream();
	auto response_stream = std::make_shared<std::ostringstream>();
	try
	{
		in_stream | http::Post("https://api.openai.com/v1/audio/transcriptions", {{"model", model_to_string(impl().m_model)}, {"response_format", "text"}}, "file", DefaultFileName("audio.mp3"), impl().m_api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error during transcription"));
	}
    auto cont = emit_message(document::Document{});
    if (cont == continuation::stop) return cont;
    cont = emit_message(document::Text{response_stream->str()});
    if (cont == continuation::stop) return cont;
    return emit_message(document::CloseDocument{});
}

} // namespace openai
} // namespace docwire
