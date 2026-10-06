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

#include "http_listener.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/config.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "httplib_patched.h"
#include "log_scope.h"
#include "make_error.h"
#include "throw_if.h"

namespace docwire::detail
{

namespace
{

httplib::StatusCode to_httplib_status(http_status_code status)
{
    switch (status)
    {
        case http_status_code::internal_server_error:
            return httplib::StatusCode::InternalServerError_500;
        case http_status_code::ok:
        default:
            return httplib::StatusCode::OK_200;
    }
}

} // anonymous namespace

struct http_listener::impl
{
    std::unique_ptr<httplib::Server> server;
    std::optional<ssl_certificate> certificate;
    std::function<void(std::exception_ptr)> error_handler;
    std::string address;
    std::uint16_t port = 0;
    std::size_t thread_num = 0;

    explicit impl(http_listener::options input_options)
        : error_handler{std::move(input_options.error_handler)}
        , address{std::move(input_options.address)}
        , port{input_options.port}
        , thread_num{input_options.thread_num}
    {
        log_scope(address, port, thread_num);

        if (input_options.certificate)
        {
            certificate.emplace(std::move(*input_options.certificate));
        }

        if (certificate)
        {
            server = std::make_unique<httplib::SSLServer>([this](httplib::tls::ctx_t tls_context)
            {
                certificate->apply_to(tls_context);
                return true;
            });
        }
        else
        {
            server = std::make_unique<httplib::Server>();
        }

        if (thread_num > 0)
        {
            // NOTE: httplib takes ownership of the returned task queue.
            server->new_task_queue = [this] { return new httplib::ThreadPool(thread_num); };
        }

        server->set_payload_max_length(input_options.body_limit);

        server->set_exception_handler([this](const httplib::Request&, httplib::Response& response, std::exception_ptr error)
        {
            error_handler(error);
            try
            {
                if (error)
                    std::rethrow_exception(error);
            }
            catch (const std::exception&)
            {
                response.status = httplib::StatusCode::InternalServerError_500;
                response.set_content("Internal Server Error", "text/plain");
            }
        });

    }

    void add_route(const std::string& path_pattern, http_route_handler handler)
    {
        server->Post(path_pattern.c_str(),
            [this, handler = std::move(handler)](const httplib::Request& httplib_request, httplib::Response& httplib_response)
        {
            http_request request;
            request.body = httplib_request.body;
            if (httplib_request.has_header("Content-Type"))
            {
                std::string content_type_header = httplib_request.get_header_value("Content-Type");
                const auto semicolon_pos = content_type_header.find(';');
                std::string media_type = (semicolon_pos != std::string::npos)
                    ? content_type_header.substr(0, semicolon_pos)
                    : content_type_header;
                boost::algorithm::trim(media_type);
                request.content_type = std::move(media_type);
            }

            http_response response;
            try
            {
                handler(request, response);
            }
            catch (const std::exception&)
            {
                response.status = http_status_code::internal_server_error;
                response.content = "Internal Server Error";
                response.error = std::current_exception();
            }

            if (response.error)
                error_handler(response.error);

            httplib_response.status = to_httplib_status(response.status);
            httplib_response.set_content(response.content, response.content_type.c_str());
        });
    }

    BOOST_NOINLINE void httplib_listen_noninlined()
    {
        log_scope();
        server->listen_after_bind();
    }

    BOOST_NOINLINE void httplib_stop_noninlined()
    {
        log_scope();
        if (server && server->is_running())
            server->stop();
    }

    void listen_after_bind()
    {
        log_scope();

        throw_if(!server->bind_to_port(address.c_str(), port),
                 "Failed to bind HTTP server", address, port);

        httplib_listen_noninlined();
    }

    void wait_until_ready()
    {
        if (server)
            server->wait_until_ready();
    }

    void stop()
    {
        log_scope();
        httplib_stop_noninlined();
    }
};

http_listener::http_listener(http_listener::options opts)
    : m_impl{std::make_unique<impl>(std::move(opts))}
{
}

http_listener::~http_listener() = default;
http_listener::http_listener(http_listener&& other) noexcept = default;
http_listener& http_listener::operator=(http_listener&& other) noexcept = default;

void http_listener::post(const std::string& path_pattern, http_route_handler handler)
{
    m_impl->add_route(path_pattern, std::move(handler));
}

void http_listener::listen_after_bind()
{
    m_impl->listen_after_bind();
}

void http_listener::wait_until_ready()
{
    m_impl->wait_until_ready();
}

void http_listener::stop()
{
    m_impl->stop();
}

} // namespace docwire::detail
