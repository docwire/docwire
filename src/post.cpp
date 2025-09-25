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
#include "error_tags.h"
#include "log.h"
#include "throw_if.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <sstream>
#include "version.h"

namespace docwire
{

template<>
struct pimpl_impl<http::Post> : pimpl_impl_base
{
	pimpl_impl(const std::string& url, const std::optional<std::map<std::string, std::string>> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token, http::ssl_verify_peer ssl_verify_peer_v = {true})
		: m_url(url), m_form(form), m_pipe_field_name(pipe_field_name), m_default_file_name(default_file_name), m_oauth2_bearer_token(oauth2_bearer_token), m_ssl_verify_peer(ssl_verify_peer_v.v)
	{}
	std::string m_url;
	std::optional<std::map<std::string, std::string>> m_form;
	std::string m_pipe_field_name;
	DefaultFileName m_default_file_name;
	std::string m_oauth2_bearer_token;
	bool m_ssl_verify_peer;
};

namespace http
{

Post::Post(const std::string& url, const std::string& oauth2_bearer_token, ssl_verify_peer ssl_verify_peer_v)
	: with_pimpl<Post>(url, std::nullopt, "", DefaultFileName{""}, oauth2_bearer_token, ssl_verify_peer_v)
{
}

Post::Post(const std::string& url, const std::map<std::string, std::string>& form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token, ssl_verify_peer ssl_verify_peer_v)
	: with_pimpl<Post>(url, form, pipe_field_name, default_file_name, oauth2_bearer_token, ssl_verify_peer_v)
{
}

continuation Post::operator()(message_ptr msg, const message_callbacks& emit_message)
try
{
	docwire_log_func();
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	docwire_log(debug) << "data_source received";
	const data_source& data = msg->get<data_source>();
	std::shared_ptr<std::istream> in_stream = data.istream();

	auto parse_url = [](const std::string& url_str) -> std::tuple<std::string, std::string, int, std::string> {
		const std::string proto_end("://");
		auto proto_it = std::search(url_str.begin(), url_str.end(), proto_end.begin(), proto_end.end());
		if (proto_it == url_str.end())
			throw make_error("Invalid URL: " + url_str, errors::program_logic{});
		std::string protocol = {url_str.begin(), proto_it};
		std::string::const_iterator host_start = proto_it + proto_end.length();

		auto path_it = std::find(host_start, url_str.end(), '/');
		std::string host_and_port_str = {host_start, path_it};
		std::string path = {path_it, url_str.end()};
		if (path.empty()) path = "/";

		std::string host;
		int port = 0; // 0 means default port for protocol

		auto port_separator_it = std::find(host_and_port_str.begin(), host_and_port_str.end(), ':');
		if (port_separator_it != host_and_port_str.end()) {
			host = {host_and_port_str.begin(), port_separator_it};
			std::string port_str = {port_separator_it + 1, host_and_port_str.end()};
			try {
				port = std::stoi(port_str);
			} catch (const std::exception& e) {
				throw make_error("Invalid port in URL: " + url_str, errors::program_logic{});
			}
		} else {
			host = host_and_port_str;
		}
		return {protocol, host, port, path};
	};

	auto [protocol, host, port, path] = parse_url(impl().m_url);

	std::unique_ptr<httplib::SSLClient> ssl_client;
	std::unique_ptr<httplib::Client> client;

	if (protocol == "https")
	{
		ssl_client = make_unique<httplib::SSLClient>(host, port == 0 ? 443 : port);
		ssl_client->enable_server_certificate_verification(impl().m_ssl_verify_peer);
	}
	else if (protocol == "http")
	{
		client = make_unique<httplib::Client>(host, port == 0 ? 80 : port);
	}
	else
	{
		throw make_error("Unsupported protocol: " + protocol, errors::program_logic{});
	}

	httplib::Headers headers;
	headers.emplace("User-Agent", std::string("DocWire SDK/") + VERSION);
	if (!impl().m_oauth2_bearer_token.empty())
	{
		headers.emplace("Authorization", "Bearer " + impl().m_oauth2_bearer_token);
	}

	std::stringstream data_stream;
	data_stream << in_stream->rdbuf();
	std::string body_str = data_stream.str();
	
	httplib::Result res;

	if (impl().m_form)
	{
		httplib::UploadFormDataItems items;
		for (auto f: *impl().m_form)
			items.push_back({f.first, f.second, "", ""});

		std::optional<file_extension> extension = data.file_extension();
		std::string content_type = "application/octet-stream";
		if (!extension)
		{
			if (auto mt = data.highest_confidence_mime_type())
			{
				extension = content_type::by_file_extension::to_extension(*mt);
				content_type = mt->v;
			}
		}

		std::filesystem::path file_name_path = !extension ? impl().m_default_file_name.v : std::filesystem::path{std::string{"file"} + extension->string()};
		items.push_back({impl().m_pipe_field_name, body_str, file_name_path.string(), content_type});
		if (ssl_client)
			res = ssl_client->Post(path, headers, items);
		else
			res = client->Post(path, headers, items);
	}
	else
	{
		std::string content_type = "application/octet-stream";
		if (auto mt = data.highest_confidence_mime_type())
		{
			content_type = mt->v;
		}
		if (ssl_client)
			res = ssl_client->Post(path, headers, body_str, content_type);
		else
			res = client->Post(path, headers, body_str, content_type);
	}

	if (!res)
	{
		httplib::Error err = res.error();
		std::string error_msg = "HTTP request failed: " + httplib::to_string(err);
		throw make_error(error_msg, errors::network_failure{});
	}

	throw_if (res->status < 200 || res->status > 299, "Server returned an error status code", res->status, res->body);

	return emit_message(data_source{seekable_stream_ptr{std::make_shared<std::stringstream>(res->body)}});
}
catch (const std::exception&)
{
	std::throw_with_nested(make_error(impl().m_url));
}

} // namespace http
} // namespace docwire
