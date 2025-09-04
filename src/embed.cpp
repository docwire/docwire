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

#include "embed.h"

#include <boost/json.hpp>
#include "error_tags.h"
#include "input.h"
#include "make_error.h"
#include "output.h"
#include "post.h"
#include "ai_elements.h"
#include "throw_if.h"
#include <sstream>

namespace docwire
{

using namespace openai;

template<>
struct pimpl_impl<openai::embed> : pimpl_impl_base
{
	pimpl_impl(const std::string& api_key, openai::embed::model model)
		: m_api_key(api_key), m_model(model) {}
	std::string m_api_key;
	openai::embed::model m_model;
};

}

namespace docwire
{
namespace openai
{

embed::embed(const std::string& api_key, model model)
	: with_pimpl<embed>(api_key, model)
{
}

namespace
{

std::string model_to_string(embed::model model)
{
	switch (model)
	{
		case embed::model::text_embedding_3_small: return "text-embedding-3-small";
		case embed::model::text_embedding_3_large: return "text-embedding-3-large";
		case embed::model::text_embedding_ada_002: return "text-embedding-ada-002";
		default: throw make_error("Unknown model", model, errors::program_logic{});
	}
}

std::string prepare_query(const std::string& input, embed::model model)
{
	boost::json::object query
	{
		{ "model", model_to_string(model) },
		{ "input", input }
	};
	return boost::json::serialize(query);
}

std::string post_request(const std::string& query, const std::string& api_key)
{
	std::ostringstream response_stream{};
	try
	{
		std::stringstream { query } | http::Post("https://api.openai.com/v1/embeddings", api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

std::vector<double> parse_response(const std::string& response)
{
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

continuation embed::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (!msg->is<data_source>())
		return emit_message(std::move(msg));
	const data_source& data = msg->get<data_source>();
	throw_if (!data.has_highest_confidence_mime_type_in({mime_type{"text/plain"}}), errors::program_logic{});
	std::string data_str = data.string();
	std::vector<double> embedding_vector = parse_response(post_request(prepare_query(data_str, impl().m_model), impl().m_api_key));
	return emit_message(ai::embedding{std::move(embedding_vector)});
}

} // namespace openai
} // namespace docwire
