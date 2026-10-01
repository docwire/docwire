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

#ifndef DOCWIRE_DETAIL_HTTP_LISTENER_H
#define DOCWIRE_DETAIL_HTTP_LISTENER_H

#include "http_export.h"
#include "ssl_certificate.h"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace docwire::detail
{

/**
 * @brief Plain value type describing an incoming HTTP request.
 */
struct http_request
{
    std::string body;
    std::string content_type;
};

/**
 * @brief HTTP status codes understood by the listener wrapper.
 */
enum class http_status_code
{
    ok = 200,
    internal_server_error = 500
};

/**
 * @brief Plain value type describing an outgoing HTTP response.
 */
struct http_response
{
    http_status_code status = http_status_code::ok;
    std::string content;
    std::string content_type = "text/plain";
    std::exception_ptr error;
};

/// Type-erased route handler. This is used only at the third-party boundary.
using http_route_handler = std::function<void(const http_request&, http_response&)>;

/**
 * @brief A thin wrapper around the third-party HTTP server implementation.
 *
 * This class owns the third-party server and thread pool, and hides them
 * behind a small, dependency-free interface. Its implementation is
 * quarantined inside a dedicated translation unit so that no third-party
 * header leaks into `docwire::http`.
 */
class DOCWIRE_HTTP_EXPORT http_listener
{
public:
    struct options
    {
        std::string address;
        std::uint16_t port = 0;
        std::size_t thread_num = 0;
        std::uint64_t body_limit = 1024ULL * 1024 * 1024;
        std::optional<ssl_certificate::pem> certificate;
        std::function<void(std::exception_ptr)> error_handler = [](std::exception_ptr) {};
    };

    explicit http_listener(options opts);
    ~http_listener();

    http_listener(http_listener&& other) noexcept;
    http_listener& operator=(http_listener&& other) noexcept;

    http_listener(const http_listener&) = delete;
    http_listener& operator=(const http_listener&) = delete;

    /// Registers a POST route for the given path or regular expression.
    void post(const std::string& path_pattern, http_route_handler handler);

    /// Runs the server event loop after binding. Blocks the calling thread.
    void listen_after_bind();

    /// Blocks until the underlying server is ready to accept connections.
    void wait_until_ready();

    /// Requests a graceful shutdown.
    void stop();

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace docwire::detail

#endif // DOCWIRE_DETAIL_HTTP_LISTENER_H
