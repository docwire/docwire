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

#ifndef DOCWIRE_HTTP_SERVER_H
#define DOCWIRE_HTTP_SERVER_H

#include "http_export.h"
#include "pimpl.h"
#include "parsing_chain.h"
#include <cstdint>
#include <string>
#include <functional>
#include <variant>

namespace docwire::http
{

struct address { std::string v; };
struct port { uint16_t v; };
struct thread_num { size_t v; };
struct cert_path { std::string v; };
struct key_path { std::string v; };
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
 * @brief A standalone HTTP server that processes requests using a user-provided pipeline.
 *
 * This server is not a `ChainElement` itself but acts as a host for processing pipelines.
 * For each registered URL, it uses a factory function to create a `ParsingChain`
 * which processes incoming requests. To enhance performance, created pipelines are
 * cached on a per-thread basis.
 */
class DOCWIRE_HTTP_EXPORT server : public with_pimpl<server>
{
public:
	/// A factory function that creates a `ParsingChain` for processing a request.
	using pipeline_factory = std::function<ParsingChain()>;
	/// A list of routes, where each route is a path (string or regex) mapped to a pipeline factory.
	using route_list = std::vector<std::pair<std::variant<std::string, regex_path>, pipeline_factory>>;
	
	/**
	 * @brief Construct a new HTTP server object
	 * @param addr The address to listen on (e.g., "0.0.0.0")
	 * @param port The port to listen on
	 * @param routes A list of routes, where each route path can be a simple string or a `docwire::http::regex_path`.
	 * @param thread_num The number of threads for the server (0 for default)
	 * @param handler A function to call for handling server errors.
	 */
	server(address addr, port port, route_list routes, thread_num thread_num = {0}, error_handler handler = {});
	
	/**
	 * @brief Construct a new HTTPS server object
	 * @param addr The address to listen on
	 * @param port The port to listen on
	 * @param cert_path Path to the SSL certificate file
	 * @param key_path Path to the SSL private key file
	 * @param routes A list of routes, where each route path can be a simple string or a `docwire::http::regex_path`.
	 * @param thread_num The number of threads for the server
	 * @param handler A function to call for handling server errors.
	 */
	server(address addr, port port, cert_path cert_path, key_path key_path, route_list routes, thread_num thread_num = {0}, error_handler handler = {});
	~server();
	server(server&&);
	server& operator=(server&&);

	/**
	 * @brief Starts the server and blocks the current thread.
	 *
	 * This method starts the underlying network event loop and will not return
	 * until stop() is called from another thread or a signal handler.
	 */
	void operator()();
	/**
	 * @brief Gracefully stops the server.
	 *
	 * This is a thread-safe method that signals the event loop to stop,
	 * allowing the operator()() call to unblock and return.
	 */
	void stop();

private:
	using with_pimpl<server>::impl;
};

} // namespace docwire::http

#endif //DOCWIRE_HTTP_SERVER_H
