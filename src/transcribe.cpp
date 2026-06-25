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

template<>
struct pimpl_impl<openai::transcribe> : pimpl_impl_base
{
	pimpl_impl(const std::string& api_key, openai::transcribe::model model)
		: m_api_key(api_key), m_model(model) {}
	std::string m_api_key;
	openai::transcribe::model m_model;
};

namespace openai
{

transcribe::transcribe(const std::string& api_key, model model)
	: with_pimpl<transcribe>(api_key, model)
{
	log_scope(model);
}

namespace
{

std::string model_to_string(transcribe::model model)
{
	switch (model)
	{
		case transcribe::model::gpt_4o_transcribe: return "gpt-4o-transcribe";
		case transcribe::model::gpt_4o_mini_transcribe: return "gpt-4o-mini-transcribe";
		case transcribe::model::whisper_1: return "whisper-1";
		default: return "?";
	}
}

} // anonymous namespace

continuation transcribe::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	log_scope();
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	log_entry();
	const data_source& data = msg->get<data_source>();
	std::shared_ptr<std::istream> in_stream = data.istream();
	auto response_stream = std::make_shared<std::ostringstream>();
	try
	{
		in_stream | http::post("https://api.openai.com/v1/audio/transcriptions", {{"model", model_to_string(impl().m_model)}, {"response_format", "text"}}, "file", default_file_name("audio.mp3"), impl().m_api_key) | response_stream;
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

} // namespace openai
} // namespace docwire
