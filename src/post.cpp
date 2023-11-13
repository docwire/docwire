/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "post.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "exception.h"
#include <fstream>
#include "log.h"
#include "parser.h"
#include <sstream>
#include "version.h"

namespace docwire
{
namespace http
{

struct Post::Implementation
{
	std::string m_url;
	std::string m_oauth2_bearer_token;
	Implementation(const std::string& url, const std::string& oauth2_bearer_token)
		: m_url(url), m_oauth2_bearer_token(oauth2_bearer_token)
	{}
};

Post::Post(const std::string& url, const std::string& oauth2_bearer_token)
	: impl(new Implementation{url, oauth2_bearer_token})
{
}

Post::Post(const Post &other)
	: impl(new Implementation(*other.impl))
{
	docwire_log_func();
}

Post::~Post()
{
}

void
Post::process(Info &info) const
{
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "TAG_FILE received";
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	if(!path && !stream)
		throw FileTagIncorrect("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;

	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(impl->m_url);
	request.setOpt(curlpp::options::UserAgent(std::string("DocWire SDK/") + VERSION));
	request.setOpt(new curlpp::options::CustomRequest("POST"));
	request.setOpt(curlpp::options::ReadFunction([in_stream](char* buf, size_t size, size_t nitems) -> size_t
	{
		docwire_log_func_with_args(size, nitems);
		in_stream->read(buf, static_cast<std::streamsize>(size * nitems));
		return in_stream->gcount();
	}));
	request.setOpt<curlpp::options::Encoding>("gzip");
	request.setOpt(curlpp::options::Upload(true));
	request.setOpt<curlpp::options::HttpHeader>({"Content-Type: application/json"});
	if (!impl->m_oauth2_bearer_token.empty())
	{
		request.setOpt<curlpp::options::HttpAuth>(CURLAUTH_BEARER);
		typedef curlpp::OptionTrait<std::string, CURLOPT_XOAUTH2_BEARER> XOAuth2Bearer;
		request.setOpt(XOAuth2Bearer(impl->m_oauth2_bearer_token));
	}
	std::stringstream response_stream;
	curlpp::options::WriteStream ws(&response_stream);
	request.setOpt(ws);
	try
	{
		request.perform();
	}
	catch (curlpp::LogicError &e)
	{
		throw RequestIncorrect("Incorrect HTTP request", e);
    }
	catch (curlpp::RuntimeError &e)
	{
		throw RequestFailed("HTTP request failed", e);
	}
	Info new_info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&response_stream}, {"name", ""}});
	emit(new_info);
	if (path)
		delete in_stream;
}

Post* Post::clone() const
{
	return new Post(*this);
}

} // namespace http
} // namespace docwire
