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

#include "post.h"

#include "content_type_by_file_extension.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include "error_tags.h"
#include "log.h"
#include "throw_if.h"
#include <sstream>
#include "version.h"

namespace docwire
{

template<>
struct pimpl_impl<http::Post> : pimpl_impl_base
{
	pimpl_impl(const std::string& url, const std::optional<std::map<std::string, std::string>> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token)
		: m_url(url), m_form(form), m_pipe_field_name(pipe_field_name), m_default_file_name(default_file_name), m_oauth2_bearer_token(oauth2_bearer_token)
	{}
	std::string m_url;
	std::optional<std::map<std::string, std::string>> m_form;
	std::string m_pipe_field_name;
	DefaultFileName m_default_file_name;
	std::string m_oauth2_bearer_token;
};

namespace http
{

Post::Post(const std::string& url, const std::string& oauth2_bearer_token)
	: with_pimpl<Post>(url, std::nullopt, "", DefaultFileName{""}, oauth2_bearer_token)
{
}

Post::Post(const std::string& url, const std::map<std::string, std::string> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token)
	: with_pimpl<Post>(url, form, pipe_field_name, default_file_name, oauth2_bearer_token)
{
}

continuation Post::operator()(message_ptr msg, const message_callbacks& emit_message)
try
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	docwire_log(debug) << "data_source received";
	const data_source& data = msg->get<data_source>();
	std::shared_ptr<std::istream> in_stream = data.istream();

	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(impl().m_url);
	request.setOpt(curlpp::options::UserAgent(std::string("DocWire SDK/") + VERSION));

	if (impl().m_form)
	{
		curlpp::Forms parts;
		for (auto f: *impl().m_form)
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
					curl_formadd(first, last, CURLFORM_PTRNAME, m_field_name.c_str(), CURLFORM_BUFFER, m_file_name.v.u8string().c_str(), CURLFORM_BUFFERPTR, m_buffer->c_str(), CURLFORM_BUFFERLENGTH, m_buffer->size(), CURLFORM_END);
				}
				std::string m_field_name;
				FileName m_file_name;
				std::shared_ptr<std::string> m_buffer;
		};
		std::optional<file_extension> extension = data.file_extension();
		if (!extension)
		{
			if (auto mt = data.highest_confidence_mime_type())
			{
				extension = content_type::by_file_extension::to_extension(*mt);
			}
		}
		FileName file_name { !extension ? impl().m_default_file_name.v : std::filesystem::path{std::string{"file"} + extension->string()} };
		parts.push_back(new FileBuffer(impl().m_pipe_field_name, file_name, std::make_shared<std::string>(data_stream.str())));
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
	if (!impl().m_oauth2_bearer_token.empty())
	{
		request.setOpt<curlpp::options::HttpAuth>(CURLAUTH_BEARER);
		typedef curlpp::OptionTrait<std::string, CURLOPT_XOAUTH2_BEARER> XOAuth2Bearer;
		request.setOpt(XOAuth2Bearer(impl().m_oauth2_bearer_token));
	}
	auto response_stream = std::make_shared<std::stringstream>();
	curlpp::options::WriteStream ws(response_stream.get());
	request.setOpt(ws);
	try
	{
		request.perform();
		auto response_code = curlpp::infos::ResponseCode::get(request);
		throw_if (response_code < 200 || response_code > 299, "Server returned an error status code", response_code, response_stream->str());
	}
	catch (curlpp::LogicError &e)
	{
		std::throw_with_nested(make_error("HTTP request is invalid", errors::program_logic{}));
    }
	catch (curlpp::RuntimeError &e)
	{
		std::throw_with_nested(make_error("HTTP request failed", errors::network_failure{}));
	}
	return emit_message(data_source{seekable_stream_ptr{response_stream}});
}
catch (const std::exception&)
{
	std::throw_with_nested(make_error(impl().m_url));
}

} // namespace http
} // namespace docwire
