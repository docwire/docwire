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
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "TAG_FILE received";
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	if(!path && !stream)
		throw LogicError("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	std::stringstream response_stream;
	try
	{
		Input(in_stream) | http::Post("https://api.openai.com/v1/audio/transcriptions", {{"model", "whisper-1"}, {"response_format", "text"}}, "file", DefaultFileName("audio.mp3"), impl->m_api_key) | Output(response_stream);
	}
	catch (const http::Post::RequestFailed& e)
	{
		if (path)
			delete in_stream;
		throw Transcribe::HttpError("Http POST failed", e);
	}
	if (path)
		delete in_stream;
	Info doc_info(StandardTag::TAG_DOCUMENT);
	emit(doc_info);
	Info text_info(StandardTag::TAG_TEXT, response_stream.str());
	emit(text_info);
	Info close_doc_info(StandardTag::TAG_CLOSE_DOCUMENT);
	emit(close_doc_info);
}

Transcribe* Transcribe::clone() const
{
	return new Transcribe(*this);
}

} // namespace openai
} // namespace docwire
