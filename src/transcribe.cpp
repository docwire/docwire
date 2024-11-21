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
#include <fstream>
#include "input.h"
#include "log.h"
#include "make_error.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{
namespace openai
{

struct Transcribe::Implementation
{
	std::string m_api_key;
};

Transcribe::Transcribe(const std::string& api_key)
	: impl(new Implementation{api_key})
{
	docwire_log_func();
}

Transcribe::Transcribe(const Transcribe& other)
	: impl(new Implementation(*other.impl))
{
	docwire_log_func();
}

Transcribe::~Transcribe()
{
}

void Transcribe::process(Info &info) const
{
	docwire_log_func();
	if (!std::holds_alternative<data_source>(info.tag))
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "data_source received";
	const data_source& data = std::get<data_source>(info.tag);
	std::shared_ptr<std::istream> in_stream = data.istream();
	auto response_stream = std::make_shared<std::ostringstream>();
	try
	{
		in_stream | http::Post("https://api.openai.com/v1/audio/transcriptions", {{"model", "whisper-1"}, {"response_format", "text"}}, "file", DefaultFileName("audio.mp3"), impl->m_api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error during transcription"));
	}
	Info doc_info(tag::Document{});
	emit(doc_info);
	Info text_info(tag::Text{response_stream->str()});
	emit(text_info);
	Info close_doc_info(tag::CloseDocument{});
	emit(close_doc_info);
}

} // namespace openai
} // namespace docwire
