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

#ifndef DF0_HTTP_POST_H
#define DF0_HTTP_POST_H

#include "pipeline/element_base.hpp"
#include "http_export.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>
#include <filesystem>
#include <map>

namespace df0
{

struct default_file_name
{
	std::filesystem::path v;
	explicit default_file_name(const std::filesystem::path& p)
		: v(p) {}
};

namespace http
{

struct ssl_verify_peer { bool v; };

template<typename auditor_type = audit::null_auditor>
class DF0_HTTP_EXPORT post
    : public pipeline::transformer_element<post<auditor_type>>,
      public with_pimpl<post<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	/**
	 * @brief Constructs the component with the default stateless auditing policy.
	 *
	 * This overload is available only when the configured auditor is stateless
	 * and therefore default-constructible.
	 *
	 * @tparam stateless_auditor_type Injected auditor type selected by this overload.
	 */
	template<audit::stateless_auditor stateless_auditor_type = auditor_type>
	post(
		const std::string& url,
		const std::string& oauth2_bearer_token = "",
		ssl_verify_peer ssl_verify_peer_v = {true})
		: post(url,
		       oauth2_bearer_token,
		       ssl_verify_peer_v,
		       audit::auditor_ref<stateless_auditor_type>{})
	{}

	post(
		const std::string& url,
		const std::string& oauth2_bearer_token,
		ssl_verify_peer ssl_verify_peer_v,
		audit::auditor_ref<auditor_type> auditor);

	/**
	 * @brief Constructs the component with the default stateless auditing policy.
	 *
	 * This overload is available only when the configured auditor is stateless
	 * and therefore default-constructible.
	 *
	 * @tparam stateless_auditor_type Injected auditor type selected by this overload.
	 */
	template<audit::stateless_auditor stateless_auditor_type = auditor_type>
	post(
		const std::string& url,
		const std::map<std::string, std::string>& form,
		const std::string& pipe_field_name,
		const default_file_name& default_file_name,
		const std::string& oauth2_bearer_token = "",
		ssl_verify_peer ssl_verify_peer_v = {true})
		: post(url,
		       form,
		       pipe_field_name,
		       default_file_name,
		       oauth2_bearer_token,
		       ssl_verify_peer_v,
		       audit::auditor_ref<stateless_auditor_type>{})
	{}

	post(
		const std::string& url,
		const std::map<std::string, std::string>& form,
		const std::string& pipe_field_name,
		const default_file_name& default_file_name,
		const std::string& oauth2_bearer_token,
		ssl_verify_peer ssl_verify_peer_v,
		audit::auditor_ref<auditor_type> auditor);

	/**
	 * @brief Executes transform for the given message; consumes msg.
	 * @see df0::message_ptr
	 * @see df0::message_callbacks
	 * @param msg Message to process.
	 * @param emit_message Emission callbacks for produced messages.
	 */
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<post<auditor_type>>::impl;
};

} // namespace http
} // namespace df0

#endif //DF0_HTTP_POST_H
