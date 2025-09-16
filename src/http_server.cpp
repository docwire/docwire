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
#include "exception_utils.h"
#include "data_source.h"
#include "input.h"
#include "output.h"
#include "throw_if.h"
#include "make_error.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <regex>

namespace docwire
{

inline std::string stringify(const boost::system::error_code& ec)
{
	return ec.message();
}

namespace http
{

namespace beast = boost::beast;
namespace http_beast = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

struct compiled_route
{
    std::string path_key;
    http::server::pipeline_factory factory;
    std::optional<std::regex> pattern;
};

namespace {

template<class Derived>
class http_session
{
    struct send_lambda
    {
        Derived& m_self;

        explicit send_lambda(Derived& self) : m_self(self) {}

        template<bool isRequest, class Body, class Fields>
        void operator()(http_beast::message<isRequest, Body, Fields>&& msg) const
        {
            auto sp = std::make_shared<http_beast::message<isRequest, Body, Fields>>(std::move(msg));
            m_self.m_res = sp;
            http_beast::async_write(
                m_self.stream(),
                *sp,
                beast::bind_front_handler(
                    &http_session::on_write,
                    m_self.derived().shared_from_this(),
                    sp->need_eof()));
        }
    };

    beast::flat_buffer m_buffer;
    http_beast::request<http_beast::string_body> m_req;
    std::shared_ptr<void> m_res;
    send_lambda m_lambda;
    std::shared_ptr<std::vector<http::compiled_route>> m_routes;
    std::shared_ptr<error_handler_func> m_error_handler;

protected:
    explicit http_session(std::shared_ptr<std::vector<http::compiled_route>> routes, std::shared_ptr<error_handler_func> handler)
        : m_lambda(derived()), m_routes(std::move(routes)), m_error_handler(std::move(handler))
    {}

    Derived& derived() { return static_cast<Derived&>(*this); }

    void do_read()
    {
        m_req = {};
        beast::get_lowest_layer(stream()).expires_after(std::chrono::seconds(30));
        http_beast::async_read(stream(), m_buffer, m_req,
            beast::bind_front_handler(
                &http_session::on_read,
                derived().shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if(ec == http_beast::error::end_of_stream)
            return do_close();
        if(ec)
        {
            report_error(ec, "http_beast::async_read() failed");
            return do_close();
        }
        handle_request(std::move(m_req), m_lambda);
    }

    void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);
        if(ec)
        {
            report_error(ec, "http_beast::async_write() failed");
            return do_close();
        }
        if(close)
            return do_close();
        m_res = nullptr;
        do_read();
    }

    void do_close() { derived().do_close(); }
    auto& stream() { return derived().stream(); }

protected:
    void report_error(beast::error_code ec, char const* what)
    {
        if(ec == net::ssl::error::stream_truncated || ec == http_beast::error::end_of_stream)
            return;
    
        (*m_error_handler)(make_error_ptr(what, ec));
    }


private:
    http_beast::response<http_beast::string_body> server_error(beast::string_view what)
    {
        http_beast::response<http_beast::string_body> res{http_beast::status::internal_server_error, m_req.version()};
        res.set(http_beast::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http_beast::field::content_type, "text/plain");
        res.keep_alive(m_req.keep_alive());
        res.body() = std::string(what);
        res.prepare_payload();
        return res;
    }

    http_beast::response<http_beast::string_body> not_found(beast::string_view target)
    {
        http_beast::response<http_beast::string_body> res{http_beast::status::not_found, m_req.version()};
        res.set(http_beast::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http_beast::field::content_type, "text/html");
        res.keep_alive(m_req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    }

    void handle_request(http_beast::request<http_beast::string_body>&& req, send_lambda& send)
    {
        if(req.method() != http_beast::verb::post)
        {
            http_beast::response<http_beast::string_body> res{http_beast::status::bad_request, req.version()};
            res.set(http_beast::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http_beast::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "Unknown HTTP-method. Only POST is supported.";
            res.prepare_payload();
            return send(std::move(res));
        }

        beast::string_view target_sv = req.target();
        auto query_pos = target_sv.find('?');
        if(query_pos != beast::string_view::npos)
            target_sv.remove_suffix(target_sv.size() - query_pos);
        std::string target(target_sv);

        server::pipeline_factory factory;
        std::string path_regex_str;

        for (const auto& route : *m_routes) {
            bool match = false;
            if (route.pattern)
            {
                match = std::regex_match(target, *route.pattern);
            }
            else
            {
                match = (target == route.path_key);
            }

            if (match) {
                factory = route.factory;
                path_regex_str = route.path_key;
                break;
            }
        }

        if (!factory) {
            return send(not_found(req.target()));
        }

        try
        {
            thread_local boost::container::flat_map<std::string, std::unique_ptr<ParsingChain>> pipelines;
            auto& pipeline_ptr = pipelines[path_regex_str];
            if (!pipeline_ptr)
                pipeline_ptr = std::make_unique<ParsingChain>(factory());
            ParsingChain& request_pipeline = *pipeline_ptr;

            auto response_messages = std::make_shared<std::vector<message_ptr>>();

            auto request_body = req.body();
            auto request_data_source = data_source(std::string(request_body));
            auto content_type_header = req[http_beast::field::content_type];
            if (!content_type_header.empty())
            {
                auto semicolon_pos = content_type_header.find(';');
                beast::string_view media_type_sv = (semicolon_pos != std::string::npos)
                    ? content_type_header.substr(0, semicolon_pos)
                    : content_type_header;
                std::string media_type_str(media_type_sv);
                boost::algorithm::trim(media_type_str);
                if (!media_type_str.empty())
                    request_data_source.add_mime_type(mime_type{media_type_str}, confidence::high);
            }

            InputChainElement{std::move(request_data_source)} | request_pipeline | OutputChainElement{response_messages};

            if (response_messages->empty())
            {
                return send(server_error("Error: The processing pipeline did not produce any output message."));
            }

            message_ptr last_msg = response_messages->back();
            if (last_msg->is<data_source>())
            {
                const auto& response_data = last_msg->get<data_source>();
                http_beast::response<http_beast::string_body> res{http_beast::status::ok, req.version()};
                res.set(http_beast::field::server, BOOST_BEAST_VERSION_STRING);
                if (auto mt = response_data.highest_confidence_mime_type())
                    res.set(http_beast::field::content_type, mt->v);
                else
                    res.set(http_beast::field::content_type, "text/plain");
                res.body() = response_data.string();
                res.prepare_payload();
                return send(std::move(res));
            }
            else if (last_msg->is<std::exception_ptr>())
            {
                (*m_error_handler)(last_msg->get<std::exception_ptr>());
                return send(server_error("Pipeline Error: " + errors::diagnostic_message(last_msg->get<std::exception_ptr>())));
            }
            else
            {
                return send(server_error("Error: The processing pipeline produced an unsupported message type as output."));
            }
        }
        catch (const std::exception& e)
        {
            (*m_error_handler)(std::current_exception());
            send(server_error("Internal Server Error: " + errors::diagnostic_message(e)));
        }
    }
};

class plain_http_session
    : public http_session<plain_http_session>
    , public std::enable_shared_from_this<plain_http_session>
{
    beast::tcp_stream m_stream;
public:
    plain_http_session(beast::tcp_stream&& stream, std::shared_ptr<std::vector<http::compiled_route>> routes, std::shared_ptr<error_handler_func> handler)
        : http_session<plain_http_session>(std::move(routes), std::move(handler)), m_stream(std::move(stream)) {}
    void run() { do_read(); }
    void do_close()
    {
        beast::error_code ec;
        m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);

        // When the peer closes the connection, performing a shutdown on our
        // end will fail with `not_connected` error. We can safely ignore it.
        if(ec && ec != beast::errc::not_connected)
            report_error(ec, "m_stream.socket().shutdown() failed");
    }
    beast::tcp_stream& stream() { return m_stream; }
};

class ssl_http_session
    : public http_session<ssl_http_session>
    , public std::enable_shared_from_this<ssl_http_session>
{
    beast::ssl_stream<beast::tcp_stream> m_stream;
public:
    ssl_http_session(tcp::socket&& socket, ssl::context& ctx, std::shared_ptr<std::vector<http::compiled_route>> routes, std::shared_ptr<error_handler_func> handler)
        : http_session<ssl_http_session>(std::move(routes), std::move(handler)), m_stream(std::move(socket), ctx) {}

    void run()
    {
        beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(30));
        m_stream.async_handshake(ssl::stream_base::server, beast::bind_front_handler(&ssl_http_session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec)
    {
        if(ec)
        {
            report_error(ec, "m_stream.async_handshake() failed");
            return do_close();
        }
        do_read();
    }

    void do_close()
    {
        beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(30));
        m_stream.async_shutdown(beast::bind_front_handler(&ssl_http_session::on_shutdown, shared_from_this()));
    }

    void on_shutdown(beast::error_code ec) { if(ec) report_error(ec, "m_stream.async_shutdown() failed"); } // Just log, no do_close()
    beast::ssl_stream<beast::tcp_stream>& stream() { return m_stream; }
};

class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& m_ioc;
    tcp::acceptor m_acceptor;
    std::optional<std::reference_wrapper<ssl::context>> m_ssl_ctx;
    std::shared_ptr<std::vector<http::compiled_route>> m_routes;
    std::shared_ptr<error_handler_func> m_error_handler;

public:
    listener(net::io_context& ioc, tcp::endpoint endpoint, std::optional<std::reference_wrapper<ssl::context>> ssl_ctx, std::shared_ptr<std::vector<http::compiled_route>> routes, std::shared_ptr<error_handler_func> handler)
        : m_ioc(ioc), m_acceptor(net::make_strand(ioc)), m_ssl_ctx(ssl_ctx), m_routes(std::move(routes)), m_error_handler(std::move(handler))
    {
        beast::error_code ec;
        m_acceptor.open(endpoint.protocol(), ec);
        throw_if(ec, "m_acceptor.open() failed", ec);
        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
        throw_if(ec, "m_acceptor.set_option(reuse_address) failed", ec);
        m_acceptor.bind(endpoint, ec);
        throw_if(ec, "m_acceptor.bind() failed", ec);
        m_acceptor.listen(net::socket_base::max_listen_connections, ec);
        throw_if(ec, "m_acceptor.listen() failed", ec);
    }

    void run() { do_accept(); }

private:
    void do_accept()
    {
        m_acceptor.async_accept(net::make_strand(m_ioc), beast::bind_front_handler(&listener::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if(ec) { (*m_error_handler)(make_error_ptr("m_acceptor.async_accept() failed", ec)); }
        else
        {
            if (m_ssl_ctx)
                std::make_shared<ssl_http_session>(std::move(socket), m_ssl_ctx->get(), m_routes, m_error_handler)->run();
            else
                std::make_shared<plain_http_session>(beast::tcp_stream(std::move(socket)), m_routes, m_error_handler)->run();
        }
        do_accept();
    }
};

} // namespace


} // namespace http

template<>
struct pimpl_impl<http::server> : pimpl_impl_base
{
	pimpl_impl(http::address addr, http::port port, http::server::route_list routes, http::thread_num thread_num, http::cert_path cert_path, http::key_path key_path, http::error_handler handler)
		: m_addr(addr.v), m_port(port.v), m_cert_path(cert_path.v), m_key_path(key_path.v), m_error_handler(std::make_shared<http::error_handler_func>(std::move(handler.v))), m_ioc(thread_num.v > 0 ? thread_num.v : std::thread::hardware_concurrency())
	{
        m_thread_num = thread_num.v > 0 ? thread_num.v : std::thread::hardware_concurrency();

        m_routes = std::make_shared<std::vector<http::compiled_route>>();
        m_routes->reserve(routes.size());
        for (const auto& [path_variant, factory] : routes)
        {
            std::visit(overloaded {
                [&](const std::string& path) {
                    m_routes->emplace_back(http::compiled_route{path, factory, std::nullopt});
                },
                [&](const http::regex_path& regex_p) {
                    m_routes->emplace_back(http::compiled_route{regex_p.pattern_string, factory, std::regex(regex_p.pattern_string)});
                }
            }, path_variant);
        }

        if (!m_cert_path.empty() && !m_key_path.empty())
        {
            try
            {
                m_ssl_ctx.emplace(http::ssl::context::tlsv12_server);
                m_ssl_ctx->use_certificate_chain_file(m_cert_path);
                m_ssl_ctx->use_private_key_file(m_key_path, http::ssl::context::file_format::pem);
            }
            catch (const std::exception& e)
            {
                throw make_error("Failed to load SSL certificate or key", e);
            }
        }
    }

	~pimpl_impl()
    {
        try
        {
            stop();
        }
        catch (...)
        {
            (*m_error_handler)(std::current_exception());
        }
    }

    void run()
    {
        auto const address = http::net::ip::make_address(m_addr);
        std::optional<std::reference_wrapper<http::ssl::context>> ssl_ctx_ref;
        if (m_ssl_ctx) ssl_ctx_ref.emplace(*m_ssl_ctx);

        std::make_shared<http::listener>(m_ioc, http::tcp::endpoint{address, m_port}, ssl_ctx_ref, m_routes, m_error_handler)->run();

        // Create a pool of m_thread_num - 1 worker threads. The main thread will be the m_thread_num-th worker.
        m_threads.reserve(m_thread_num > 1 ? m_thread_num - 1 : 0);
        for (size_t i = 1; i < m_thread_num; ++i)
            m_threads.emplace_back([this] { m_ioc.run(); });
        m_ioc.run();
    }

	void stop()
	{
        if (!m_ioc.stopped())
        {
		    m_ioc.stop();
            for(auto& t : m_threads)
			{
                if(t.joinable())
				{
                    t.join();
				}
			}
            m_threads.clear();
        }
	}

	std::string m_addr;
	uint16_t m_port;
	size_t m_thread_num;
	std::string m_cert_path;
	std::string m_key_path;
	std::shared_ptr<std::vector<http::compiled_route>> m_routes;
    std::shared_ptr<http::error_handler_func> m_error_handler;
    http::net::io_context m_ioc;
    std::optional<http::ssl::context> m_ssl_ctx;
    std::vector<std::thread> m_threads;
};

namespace http {
server::server(address addr, port port, route_list routes, thread_num thread_num, error_handler handler)
	: with_pimpl<server>(addr, port, std::move(routes), thread_num, cert_path{""}, key_path{""}, std::move(handler))
{}

server::server(address addr, port port, cert_path cert_path, key_path key_path, route_list routes, thread_num thread_num, error_handler handler)
	: with_pimpl<server>(addr, port, std::move(routes), thread_num, cert_path, key_path, std::move(handler))
{}

server::~server() = default;

server::server(server&&) = default;

server& server::operator=(server&&) = default;

void server::operator()()
{
    try
    {
        impl().run();
    }
    catch(const std::exception& e)
    {
		std::throw_with_nested(make_error("HTTP server failed to start"));
    }
}

void server::stop()
{
	impl().stop();
}

} // namespace http
} // namespace docwire
