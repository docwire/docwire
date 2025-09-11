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
#include <map>
#include <string>
#include <functional>

namespace docwire::http
{

struct address { std::string v; };
struct port { uint16_t v; };
struct thread_num { size_t v; };
struct cert_path { std::string v; };
struct key_path { std::string v; };

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
	/// A map from a URL path regex to a pipeline factory.
	using pipeline_factory_map = std::map<std::string, pipeline_factory>;
	
	/**
	 * @brief Construct a new HTTP server object
	 * @param addr The address to listen on (e.g., "0.0.0.0")
	 * @param port The port to listen on
	 * @param factories A map from a URL path regex to a function that creates a `ParsingChain` to process requests.
	 * @param thread_num The number of threads for the server (0 for default)
	 */
	server(address addr, port port, pipeline_factory_map factories, thread_num thread_num = {0});
	
	/**
	 * @brief Construct a new HTTPS server object
	 * @param addr The address to listen on
	 * @param port The port to listen on
	 * @param cert_path Path to the SSL certificate file
	 * @param key_path Path to the SSL private key file
	 * @param factories A map from a URL path regex to a function that creates a `ParsingChain` to process requests.
	 * @param thread_num The number of threads for the server
	 */
	server(address addr, port port, cert_path cert_path, key_path key_path, pipeline_factory_map factories, thread_num thread_num = {0});
	~server();

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
