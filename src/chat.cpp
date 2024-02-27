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

#include <boost/json.hpp>
#include <botan/base64.h>
#include <fstream>
#include "input.h"
#include "log.h"
#include "output.h"
#include "post.h"
#include <sstream>

namespace docwire
{
namespace openai
{

struct Chat::Implementation
{
	std::string m_system_message;
	std::string m_api_key;
	Model m_model;
	float m_temperature;
	ImageDetail m_image_detail;
};

Chat::Chat(const std::string& system_message, const std::string& api_key, Model model, float temperature, ImageDetail image_detail)
	: impl(new Implementation{system_message, api_key, model, temperature, image_detail})
{
	docwire_log_func_with_args(system_message, temperature);
}

Chat::Chat(const Chat& other)
	: impl(new Implementation(*other.impl))
{
	docwire_log_func();
}

Chat::~Chat()
{
}

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
		default: throw Chat::IncorrectArgumentValue("Incorrect image detail value");
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
	std::stringstream query_stream { query };
	std::stringstream response_stream;
	try
	{
		Input(&query_stream) | http::Post("https://api.openai.com/v1/chat/completions", api_key) | Output(response_stream);
	}
	catch (const http::Post::RequestFailed& e)
	{
		throw Chat::HttpError("Http POST failed: " + query, e);
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
		throw Chat::ParseResponseError("Error parsing response: " + response, e);
	}
}

bool has_txt_extension(const std::string& fn)
{
	docwire_log_func_with_args(fn);
	return std::filesystem::path(fn).extension().string() == ".txt";
}

} // anonymous namespace

void Chat::process(Info &info) const
{
	docwire_log_func();
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "TAG_FILE received";
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	std::optional<std::string> name = info.getAttributeValue<std::string>("name");
	if(!path && !stream)
		throw LogicError("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	UserMsgType user_msg_type;
	std::stringstream data_stream;
	if ((path && has_txt_extension(*path)) || (name && has_txt_extension(*name)))
	{
		docwire_log(debug) << "Filename extension shows it is a text file.";
		user_msg_type = UserMsgType::text;
		data_stream << in_stream->rdbuf();
	}
	else
	{
		docwire_log(debug) << "Filename extension shows it is not a text file. Let's assume it is an image.";
		user_msg_type = UserMsgType::image_url;
		std::vector<uint8_t> inputData(
			std::istreambuf_iterator<char>{*in_stream},
			std::istreambuf_iterator<char>{}
		);
		std::string base64Encoded = Botan::base64_encode(inputData);
		docwire_log_var(base64Encoded);
		data_stream << "data:image/*;base64," << base64Encoded;
	}
	if (path)
		delete in_stream;
	std::stringstream content_stream { parse_response(post_request(prepare_query(impl->m_system_message, user_msg_type, data_stream.str(), impl->m_model, impl->m_temperature, impl->m_image_detail), impl->m_api_key)) + '\n' };
	Info new_info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&content_stream}, {"name", ""}});
	emit(new_info);
}

Chat* Chat::clone() const
{
	return new Chat(*this);
}

} // namespace openai
} // namespace docwire
