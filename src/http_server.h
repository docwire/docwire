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

#ifndef DOCWIRE_HTTP_SERVER_H
#define DOCWIRE_HTTP_SERVER_H

#include "data_source.h"
#include "http_listener.h"
#include "ssl_certificate.h"
#include "http_export.h"
#include "pipeline/input_element.hpp"
#include "log_scope.h"
#include "make_error.h"
#include "message.h"
#include "pipeline/output_element.hpp"
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace docwire::http
{

struct address { std::string v; };
struct port { uint16_t v; };
struct thread_num { size_t v; };
struct cert_path { std::string v; };
struct key_path { std::string v; };

struct certificate_info { std::string key; std::string cert; };
struct body_limit { uint64_t v; };
using error_handler_func = std::function<void(std::exception_ptr)>;
struct error_handler { error_handler_func v = [](std::exception_ptr){}; };

/**
 * @brief A wrapper for a regex route pattern.
 *
 * This struct is used to explicitly mark a route path as a regular expression.
 */
struct regex_path
{
    std::string pattern_string;

    explicit regex_path(std::string s) : pattern_string(std::move(s)) {}
};

/**
 * @brief A single compile-time route definition.
 *
 * The route factory is stored by value so that the pipeline type is preserved
 * and never type-erased. Each route entry type produces its own specialized
 * request handler and its own per-thread pipeline cache.
 *
 * @tparam Factory A callable returning a chain (pipeline) of `chain_element`s.
 */
template <typename Factory>
struct route
{
    std::variant<std::string, regex_path> path;
    Factory factory;
};

template <typename Factory>
route(const std::string&, Factory) -> route<Factory>;

template <typename Factory>
route(const char*, Factory) -> route<Factory>;

template <typename Factory>
route(regex_path, Factory) -> route<Factory>;

/**
 * @brief A standalone HTTP server that processes requests using statically
 *        typed, compile-time route pipelines.
 *
 * Unlike the previous implementation, this server does not type-erase its
 * pipelines behind `std::function`. Each route entry given to the constructor
 * is stored by type, and its handler is generated specifically for that
 * pipeline type. Pipelines are cached per-thread, per-route-type.
 */
template <typename... Routes>
class server
{
public:
    server(address addr, port port, Routes... routes)
        : server{std::move(addr), std::move(port), std::nullopt,
                 thread_num{}, error_handler{}, body_limit{1024 * 1024 * 1024},
                 std::move(routes)...}
    {
    }

    server(address addr, port port, certificate_info cert, Routes... routes)
        : server{std::move(addr), std::move(port), std::optional<certificate_info>{std::move(cert)},
                 thread_num{}, error_handler{}, body_limit{1024 * 1024 * 1024},
                 std::move(routes)...}
    {
    }

    server(address addr, port port, thread_num thread_count, error_handler handler,
           body_limit limit, Routes... routes)
        : server{std::move(addr), std::move(port), std::nullopt,
                 std::move(thread_count), std::move(handler), std::move(limit),
                 std::move(routes)...}
    {
    }

    server(address addr, port port, certificate_info cert, thread_num thread_count,
           error_handler handler, body_limit limit, Routes... routes)
        : server{std::move(addr), std::move(port), std::optional<certificate_info>{std::move(cert)},
                 std::move(thread_count), std::move(handler), std::move(limit),
                 std::move(routes)...}
    {
    }

    /**
     * @brief Starts the server and blocks the current thread.
     *
     * This method starts the underlying network event loop and will not return
     * until stop() is called from another thread or a signal handler.
     */
    void operator()()
    {
        DOCWIRE_LOG_SCOPE();
        try
        {
            m_listener.listen_after_bind();
        }
        catch (const std::exception&)
        {
            std::throw_with_nested(DOCWIRE_MAKE_ERROR("HTTP server failed to start"));
        }
    }

    /**
     * @brief Blocks until the server is ready to accept connections.
     */
    void wait_until_ready()
    {
        DOCWIRE_LOG_SCOPE();
        m_listener.wait_until_ready();
    }

    /**
     * @brief Gracefully stops the server.
     *
     * This is a thread-safe method that signals the event loop to stop,
     * allowing the operator()() call to unblock and return.
     */
    void stop()
    {
        DOCWIRE_LOG_SCOPE();
        m_listener.stop();
    }

private:
    /// The eponymous "private constructor" used by all public overloads.
    server(address addr, port port, std::optional<certificate_info> cert,
           thread_num thread_count, error_handler handler, body_limit limit, Routes... routes)
        : m_listener{make_options(std::move(addr), port, std::move(cert),
                                  std::move(thread_count), std::move(handler), limit)}
    {
        (register_route(routes), ...);
    }

    static ::docwire::detail::http_listener::options make_options(
        address addr, port port_v, std::optional<certificate_info> cert,
        thread_num thread_count, error_handler handler, body_limit limit)
    {
        ::docwire::detail::http_listener::options opts;
        opts.address = std::move(addr.v);
        opts.port = port_v.v;
        opts.thread_num = thread_count.v;
        opts.body_limit = limit.v;
        opts.error_handler = std::move(handler.v);
        if (cert)
            opts.certificate.emplace(::docwire::detail::ssl_certificate::pem{
                std::move(cert->key), std::move(cert->cert)});
        return opts;
    }

    template <typename Factory>
    void register_route(route<Factory>& entry)
    {
        const std::string path = path_string(entry);
        m_listener.post(path, make_handler(std::move(entry.factory)));
    }

    template <typename Factory>
    static std::string path_string(const route<Factory>& entry)
    {
        return std::visit([](const auto& p) -> std::string
        {
            using T = std::decay_t<decltype(p)>;
            if constexpr (std::is_same_v<T, regex_path>)
                return p.pattern_string;
            else
                return p;
        }, entry.path);
    }

    template <typename Factory>
    static ::docwire::detail::http_route_handler make_handler(Factory factory)
    {
        using pipeline_type = std::decay_t<std::invoke_result_t<Factory>>;

        struct route_cache_state
        {
            Factory factory;
        };

        auto state = std::make_shared<route_cache_state>(std::move(factory));

        return [state](
            const ::docwire::detail::http_request& request,
            ::docwire::detail::http_response& response)
        {
            // Each route registration owns a distinct `route_cache_state`.
            // The thread-local cache is keyed by that object, not by the
            // Factory type, so two routes with the same factory closure type
            // receive independent pipelines.
            using cache_key = std::shared_ptr<route_cache_state>;

            static thread_local std::unordered_map<cache_key, pipeline_type> pipelines;

            auto it = pipelines.find(state);
            if (it == pipelines.end())
            {
                it = pipelines.emplace(state, state->factory()).first;
            }
            pipeline_type& pipeline = it->second;

            auto response_messages = std::make_shared<std::vector<message_ptr>>();

            auto request_data_source = data_source{std::string{request.body}};
            if (!request.content_type.empty())
                request_data_source.add_mime_type(mime_type{request.content_type}, confidence::high);

            // NOTE: The output is attached to the cached pipeline *before* the
            // input generator is piped in, so that the intermediate chain is
            // not considered "complete" and does not run prematurely. Only the
            // fully assembled chain runs, exactly once.
            auto pipeline_with_output = pipeline | pipeline::output_element{response_messages};
            pipeline::input_element{std::move(request_data_source)} | pipeline_with_output;

            if (response_messages->empty())
            {
                response.status = ::docwire::detail::http_status_code::internal_server_error;
                response.content = "Error: The processing pipeline did not produce any output message.";
                return;
            }

            message_ptr last_message = response_messages->back();
            if (last_message->is<data_source>())
            {
                const auto& response_data = last_message->get<data_source>();
                response.status = ::docwire::detail::http_status_code::ok;
                response.content = response_data.string();
                if (auto content_type = response_data.highest_confidence_mime_type())
                    response.content_type = content_type->v;
            }
            else if (last_message->is<std::exception_ptr>())
            {
                response.error = last_message->get<std::exception_ptr>();
                response.status = ::docwire::detail::http_status_code::internal_server_error;
                response.content = "Internal Server Error";
            }
            else
            {
                response.status = ::docwire::detail::http_status_code::internal_server_error;
                response.content = "Error: The processing pipeline produced an unsupported message type as output.";
            }
        };
    }

    ::docwire::detail::http_listener m_listener;
};

/**
 * @brief Generates a self-signed certificate and private key.
 *
 * @param common_name The common name for the certificate (e.g., "localhost" or an IP address).
 * @param country The country name for the certificate.
 * @param organization The organization name for the certificate.
 * @return A certificate_info struct containing the private key and certificate in PEM format.
 */
DOCWIRE_HTTP_EXPORT certificate_info generate_self_signed_cert(const std::string& common_name, const std::string& country, const std::string& organization);

} // namespace docwire::http

#endif //DOCWIRE_HTTP_SERVER_H
