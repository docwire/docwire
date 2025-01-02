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

#include "chat.h"

#include "base64.h"
#include <boost/json.hpp>
#include "error_tags.h"
#include "input.h"
#include "log.h"
#include "log_file_extension.h" // IWYU pragma: keep
#include <magic_enum/magic_enum_iostream.hpp>
#include "make_error.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{

using namespace openai;

template<>
struct pimpl_impl<openai::Chat> : pimpl_impl_base
{
	pimpl_impl(const std::string& system_message, const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
		: m_system_message(system_message), m_api_key(api_key), m_model(model), m_temperature(temperature), m_image_detail(image_detail) {}
	std::string m_system_message;
	std::string m_api_key;
	Model m_model;
	float m_temperature;
	ImageDetail m_image_detail;
};

}

namespace docwire
{
namespace openai
{

Chat::Chat(const std::string& system_message, const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: with_pimpl<Chat>(system_message, api_key, model, temperature, image_detail)
{
	docwire_log_func_with_args(system_message, temperature);
}

Chat::Chat(Chat&&) = default;

Chat::~Chat()
{
}

using magic_enum::ostream_operators::operator<<;

namespace
{

std::string model_to_string(Model model)
{
	switch (model)
	{
		case Model::gpt35_turbo: return "gpt-3.5-turbo";
		case Model::gpt35_turbo_0125: return "gpt-3.5-turbo-0125";
		case Model::gpt35_turbo_1106: return "gpt-3.5-turbo-1106";
		case Model::gpt4: return "gpt-4";
		case Model::gpt4_0613: return "gpt-4-0613";
		case Model::gpt4_32k: return "gpt-4-32k";
		case Model::gpt4_32k_0613: return "gpt-4-32k-0613";
		case Model::gpt4_turbo_preview: return "gpt-4-turbo-preview";
		case Model::gpt4_0125_preview: return "gpt-4-0125-preview";
		case Model::gpt4_1106_preview: return "gpt-4-1106-preview";
		case Model::gpt4_vision_preview: return "gpt-4-vision-preview";
		case Model::gpt4_1106_vision_preview: return "gpt-4-1106-vision-preview";
		default: return "?";
	}
}

std::string image_detail_to_string(ImageDetail image_detail)
{
	switch (image_detail)
	{
		case ImageDetail::low: return "low";
		case ImageDetail::high: return "high";
		case ImageDetail::automatic: return "auto";
		default: throw make_error("Unexpected image detail value", image_detail, errors::program_logic{});
	}
}

enum class UserMsgType { text, image_url };

std::string prepare_query(const std::string& system_msg, UserMsgType user_msg_type, const std::string& user_msg, Model model, float temperature, ImageDetail image_detail)
{
	docwire_log_func_with_args(system_msg, user_msg);
	boost::json::object query
	{
		{ "model", model_to_string(model) },
		{ "max_tokens",
			model == Model::gpt4_vision_preview ?
				1024 :
				boost::json::value(nullptr)
		},
		{ "messages", boost::json::array
			{
				boost::json::object {{ "role", "system" }, {"content", system_msg}},
				boost::json::object {{ "role", "user" }, {"content",
					user_msg_type == UserMsgType::text ?
						boost::json::value(user_msg) :
						boost::json::array {
							boost::json::object {
								{ "type", "image_url"},
								{ "image_url", boost::json::object {
									{ "url", user_msg },
									{ "detail", image_detail_to_string(image_detail) }
								}}
							}
						}
				}}
			}
		},
		{ "temperature", temperature }
	};
	return boost::json::serialize(query);
}

std::string post_request(const std::string& query, const std::string& api_key)
{
	docwire_log_func_with_args(query);
	std::ostringstream response_stream{};
	try
	{
		std::stringstream { query } | http::Post("https://api.openai.com/v1/chat/completions", api_key) | response_stream;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(query));
	}
	return response_stream.str();
}

std::string parse_response(const std::string& response)
{
	docwire_log_func_with_args(response);
	try
	{
		boost::json::value response_val = boost::json::parse(response);
		return response_val.as_object()["choices"].as_array()[0].as_object()["message"].as_object()["content"].as_string().c_str();
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error(response));
	}
}

bool has_txt_extension(const file_extension& extension)
{
	docwire_log_func_with_args(extension);
	return extension == file_extension{".txt"};
}

} // anonymous namespace

void Chat::process(Info& info)
{
	docwire_log_func();
	if (!std::holds_alternative<data_source>(info.tag))
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "data_source received";
	const data_source& data = std::get<data_source>(info.tag);
	UserMsgType user_msg_type;
	std::string data_str;
	if (data.file_extension() && has_txt_extension(*data.file_extension()))
	{
		docwire_log(debug) << "Filename extension shows it is a text file.";
		user_msg_type = UserMsgType::text;
		data_str = data.string();
	}
	else
	{
		docwire_log(debug) << "Filename extension shows it is not a text file. Let's assume it is an image.";
		user_msg_type = UserMsgType::image_url;
		std::span<const std::byte> input_data = data.span();
		std::string base64Encoded = base64::encode(input_data);
		docwire_log_var(base64Encoded);
		data_str = std::string{"data:image/*;base64,"} + base64Encoded;
	}
	std::string content = parse_response(post_request(prepare_query(impl().m_system_message, user_msg_type, data_str, impl().m_model, impl().m_temperature, impl().m_image_detail), impl().m_api_key)) + '\n';
	Info new_info(data_source{content});
	emit(new_info);
}

} // namespace openai
} // namespace docwire
