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
#include <curlpp/Infos.hpp>
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
	std::optional<std::map<std::string, std::string>> m_form;
	std::string m_pipe_field_name;
	DefaultFileName m_default_file_name;
	std::string m_oauth2_bearer_token;
};

Post::Post(const std::string& url, const std::string& oauth2_bearer_token)
	: impl(new Implementation{url, std::nullopt, "", DefaultFileName{""}, oauth2_bearer_token})
{
}

Post::Post(const std::string& url, const std::map<std::string, std::string> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token)
	: impl(new Implementation{url, form, pipe_field_name, default_file_name, oauth2_bearer_token})
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
	std::optional<std::string> name = info.getAttributeValue<std::string>("name");
	if(!path && !stream)
		throw FileTagIncorrect("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;

	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(impl->m_url);
	request.setOpt(curlpp::options::UserAgent(std::string("DocWire SDK/") + VERSION));

	if (impl->m_form)
	{
		curlpp::Forms parts;
		for (auto f: *impl->m_form)
			parts.push_back(new curlpp::FormParts::Content(f.first, f.second));
		std::stringstream data_stream;
		data_stream << in_stream->rdbuf();
		struct FileName
		{
			std::filesystem::path v;
			explicit FileName(const std::filesystem::path& fn)
				: v(fn) {}
		};
		class FileBuffer : public curlpp::FormPart
		{
			public:
				FileBuffer(const std::string& field_name, const FileName& file_name, std::shared_ptr<std::string> buffer)
					: FormPart(field_name), m_field_name(field_name), m_file_name(file_name), m_buffer(buffer)
				{
				}
				virtual ~FileBuffer()
				{
				}
				virtual FileBuffer* clone() const
				{
					return new FileBuffer(*this);
				}
			private:
				void add(::curl_httppost ** first, ::curl_httppost ** last)
				{
					curl_formadd(first, last, CURLFORM_PTRNAME, m_field_name.c_str(), CURLFORM_BUFFER, m_file_name.v.c_str(), CURLFORM_BUFFERPTR, m_buffer->c_str(), CURLFORM_BUFFERLENGTH, m_buffer->size(), CURLFORM_END);
				}
				std::string m_field_name;
				FileName m_file_name;
				std::shared_ptr<std::string> m_buffer;
		};
		FileName file_name { name ? std::filesystem::path(*name) : (path ? std::filesystem::path(*path).filename() : impl->m_default_file_name.v) };
		parts.push_back(new FileBuffer(impl->m_pipe_field_name, file_name, std::make_shared<std::string>(data_stream.str())));
		request.setOpt(new curlpp::options::HttpPost(parts));
	}
	else
	{
		request.setOpt(new curlpp::options::CustomRequest("POST"));
		request.setOpt(curlpp::options::ReadFunction([in_stream](char* buf, size_t size, size_t nitems) -> size_t
		{
			docwire_log_func_with_args(size, nitems);
			in_stream->read(buf, static_cast<std::streamsize>(size * nitems));
			return in_stream->gcount();
		}));
		request.setOpt(curlpp::options::Upload(true));
		request.setOpt<curlpp::options::HttpHeader>({"Content-Type: application/json"});
	}
	request.setOpt<curlpp::options::Encoding>("gzip");
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
		auto response_code = curlpp::infos::ResponseCode::get(request);
		if (response_code < 200 || response_code > 299)
			throw RequestFailed("HTTP response code is " + std::to_string(response_code) + " with response " + response_stream.str());
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
