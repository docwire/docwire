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

#include "http_server.h"
#include "parsing_chain.h"
#include "log.h"
#include "exception_utils.h"
#include <drogon/drogon.h>
#include <vector>
#include "data_source.h"
#include "input.h"
#include "output.h"
#include "make_error.h"
#include <mutex>
#include <boost/algorithm/string/trim.hpp>

namespace docwire
{

template<>
struct pimpl_impl<http::server> : pimpl_impl_base
{
	pimpl_impl(http::address addr, http::port port, http::server::pipeline_factory_map factories, http::thread_num thread_num, http::cert_path cert_path, http::key_path key_path)
		: m_addr(addr.v), m_port(port.v), m_thread_num(thread_num.v), m_cert_path(cert_path.v), m_key_path(key_path.v), m_factories(std::move(factories))
	{}

	~pimpl_impl()
	{
		shutdown();
	}

	void shutdown()
	{
		std::lock_guard<std::mutex> lock(m_drogon_app_mutex);
		if (drogon::app().isRunning()) {
			drogon::app().quit();
		}
	}

	std::string m_addr;
	uint16_t m_port;
	size_t m_thread_num;
	std::string m_cert_path;
	std::string m_key_path;
	http::server::pipeline_factory_map m_factories;
	std::mutex m_drogon_app_mutex;
};

namespace http
{

server::server(address addr, port port, pipeline_factory_map factories, thread_num thread_num)
	: with_pimpl<server>(addr, port, std::move(factories), thread_num, cert_path{""}, key_path{""})
{}

server::server(address addr, port port, cert_path cert_path, key_path key_path, pipeline_factory_map factories, thread_num thread_num)
	: with_pimpl<server>(addr, port, std::move(factories), thread_num, cert_path, key_path)
{}

server::~server() = default;

void server::operator()()
{
	try
	{
		if (!impl().m_cert_path.empty() && !impl().m_key_path.empty())
			drogon::app().addListener(impl().m_addr, impl().m_port, true, impl().m_cert_path, impl().m_key_path);
		else
			drogon::app().addListener(impl().m_addr, impl().m_port);

		if (impl().m_thread_num > 0)
			drogon::app().setThreadNum(impl().m_thread_num);

		using DrogonCallback = std::function<void(const drogon::HttpResponsePtr&)>;

		for (const auto& [path, factory] : impl().m_factories)
		{
			auto handler = [factory = factory](const drogon::HttpRequestPtr& req, DrogonCallback&& callback)
			{
				try
				{
					thread_local ParsingChain request_pipeline = factory();

					auto response_messages = std::make_shared<std::vector<message_ptr>>();

					auto request_body = req->getBody();
					auto request_data_source = data_source(request_body);
					auto content_type_header = req->getHeader("Content-Type");
					if (!content_type_header.empty())
					{
						// The Content-Type header can include parameters (e.g., "text/plain; charset=utf-8").
						// We extract only the media type part and treat it as a high-confidence hint.
						// This allows more specific detectors (e.g., signature analysis) to override it if necessary.
						auto semicolon_pos = content_type_header.find(';');
						std::string media_type_str = (semicolon_pos != std::string::npos)
							? content_type_header.substr(0, semicolon_pos)
							: content_type_header;
						boost::algorithm::trim(media_type_str);
						if (!media_type_str.empty())
							request_data_source.add_mime_type(mime_type{media_type_str}, confidence::high);
					}

					InputChainElement{std::move(request_data_source)} | request_pipeline | OutputChainElement{response_messages};

					if (response_messages->empty())
					{
						auto resp = drogon::HttpResponse::newHttpResponse();
						resp->setStatusCode(drogon::k500InternalServerError);
						resp->setBody("Error: The processing pipeline did not produce any output message.");
						callback(resp);
						return;
					}

					message_ptr last_msg = response_messages->back();
					if (last_msg->is<data_source>())
					{
						const auto& response_data = last_msg->get<data_source>();
						auto resp = drogon::HttpResponse::newHttpResponse();
						resp->setBody(response_data.string());
						if (auto mt = response_data.highest_confidence_mime_type())
							resp->setContentTypeString(mt->v);
						else
							resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
						callback(resp);
					}
					else if (last_msg->is<std::exception_ptr>())
					{
						auto resp = drogon::HttpResponse::newHttpResponse();
						resp->setStatusCode(drogon::k500InternalServerError);
						resp->setBody("Pipeline Error: " + errors::diagnostic_message(last_msg->get<std::exception_ptr>()));
						callback(resp);
					}
					else
					{
						auto resp = drogon::HttpResponse::newHttpResponse();
						resp->setStatusCode(drogon::k500InternalServerError);
						resp->setBody("Error: The processing pipeline produced an unsupported message type as output.");
						callback(resp);
					}
				}
				catch (const std::exception& e)
				{
					docwire_log(error) << "Error processing HTTP request: " << errors::diagnostic_message(e);
					auto resp = drogon::HttpResponse::newHttpResponse();
					resp->setStatusCode(drogon::k500InternalServerError);
					resp->setBody("Internal Server Error: " + errors::diagnostic_message(e));
					callback(resp);
				}
			};

			// For now, we register the handler for POST requests. This could be made configurable.
			drogon::app().registerHandlerViaRegex(path, handler, {drogon::Post});
		}
		docwire_log(info) << "Starting HTTP server on " << impl().m_addr << ":" << impl().m_port;
		drogon::app().run();
		docwire_log(info) << "HTTP server stopped.";
	}
	catch (const std::exception& e)
	{
		docwire_log(error) << "HTTP server failed to start: " << errors::diagnostic_message(e);
		std::throw_with_nested(make_error("HTTP server failed to start"));
	}
}

void server::stop()
{
	impl().shutdown();
}

} // namespace http
} // namespace docwire
