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

#include "embed.h"

#include <boost/json.hpp>
#include "error_tags.h"
#include "pipeline/input_element.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "make_error.h"
#include "pipeline/output_element.hpp"
#include "post.h"
#include "ai_elements.h"
#include "serialization_enum.h" // IWYU pragma: keep
#include "serialization_message.h" // IWYU pragma: keep
#include "throw_if.h"
#include <sstream>

namespace docwire
{

using namespace openai;

template<typename auditor_type>
struct pimpl_impl<openai::embed<auditor_type>>
    : pimpl_impl_base,
      private audit::with_auditor<auditor_type>
{
	using audit::with_auditor<auditor_type>::auditor;

	explicit pimpl_impl(
		const std::string& api_key,
		typename openai::embed<auditor_type>::model model,
		audit::auditor_ref<auditor_type> auditor)
		: audit::with_auditor<auditor_type>{auditor},
		  m_api_key(api_key),
		  m_model(model)
	{}

	std::string m_api_key;
	typename openai::embed<auditor_type>::model m_model;
};

}

namespace docwire
{
namespace openai
{

template<typename auditor_type>
embed<auditor_type>::embed(
	const std::string& api_key,
	model model,
	audit::auditor_ref<auditor_type> auditor)
	: with_pimpl<embed<auditor_type>>(api_key, model, auditor),
	  audit::with_auditor<auditor_type>(auditor)
{}

namespace
{

template<typename auditor_type>
std::string model_to_string(typename openai::embed<auditor_type>::model model)
{
	switch (model)
	{
		case openai::embed<auditor_type>::model::text_embedding_3_small: return "text-embedding-3-small";
		case openai::embed<auditor_type>::model::text_embedding_3_large: return "text-embedding-3-large";
		case openai::embed<auditor_type>::model::text_embedding_ada_002: return "text-embedding-ada-002";
		default: throw make_error("Unknown model", model, errors::program_logic{});
	}
}

template<typename auditor_type>
std::string prepare_query(
	const std::string& input,
	typename openai::embed<auditor_type>::model model,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{
		return "input"_v = input, "model"_v = model;
	}};
	boost::json::object query
	{
		{ "model", model_to_string<auditor_type>(model) },
		{ "input", input }
	};
	return boost::json::serialize(query);
}

template<typename auditor_type>
std::string post_request(
	const std::string& query,
	const std::string& api_key,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "query"_v = query; }};
	std::ostringstream response_stream{};
	try
	{
		data_source{query, mime_type{"application/json"}, confidence::highest} |
			http::post<>("https://api.openai.com/v1/embeddings", api_key) |
			response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

template<typename auditor_type>
std::vector<double> parse_response(
	const std::string& response,
	audit::auditor_ref<auditor_type> auditor)
{
	audit::scope audit_scope{auditor, [&]{ return "response"_v = response; }};
	try
	{
		boost::json::value response_val = boost::json::parse(response);
		const auto& embedding_array = response_val.as_object().at("data").as_array().at(0).as_object().at("embedding").as_array();
		std::vector<double> embedding;
		embedding.reserve(embedding_array.size());
		for (const auto& val : embedding_array)
		{
			embedding.push_back(val.as_double());
		}
		return embedding;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(response));
	}
}

} // anonymous namespace

template<typename auditor_type>
continuation embed<auditor_type>::operator()(
	message_ptr msg,
	const message_callbacks& emit_message)
{
	audit::scope audit_scope{this->auditor(), [&]{ return "msg"_v = msg; }};
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	const data_source& data = msg->get<data_source>();
	throw_if (!data.has_highest_confidence_mime_type_in({mime_type{"text/plain"}}), errors::program_logic{});
	std::string data_str = data.string();
	auto auditor_ref = audit::auditor_ref<auditor_type>(this->auditor());
	std::vector<double> embedding_vector =
		parse_response(
			post_request(
				prepare_query(data_str, impl().m_model, auditor_ref),
				impl().m_api_key,
				auditor_ref),
			auditor_ref);
	return emit_message(ai::embedding{std::move(embedding_vector)});
}

template class DOCWIRE_OPENAI_EXPORT docwire::openai::embed<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::embed<docwire::audit::legacy_log_audit>;
template class DOCWIRE_OPENAI_EXPORT docwire::openai::embed<docwire::audit::json_stream_auditor>;

} // namespace openai
} // namespace docwire
