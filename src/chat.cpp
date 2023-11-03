/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include "chat.h"

#include <boost/json.hpp>
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
	float m_temperature;
};

Chat::Chat(const std::string& system_message, const std::string& api_key, float temperature)
	: impl(new Implementation{system_message, api_key, temperature})
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

std::string prepare_query(const std::string& system_msg, const std::string& user_msg, float temperature)
{
	docwire_log_func_with_args(system_msg, user_msg);
	boost::json::object query
	{
		{ "model", "gpt-3.5-turbo" },
		{ "messages", boost::json::array
			{
				boost::json::object {{ "role", "system" }, {"content", system_msg}},
				boost::json::object {{ "role", "user" }, {"content", user_msg}}
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
	if(!path && !stream)
		throw LogicError("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	std::stringstream data_stream;
	data_stream << in_stream->rdbuf();
	if (path)
		delete in_stream;
	std::stringstream content_stream { parse_response(post_request(prepare_query(impl->m_system_message, data_stream.str(), impl->m_temperature), impl->m_api_key)) + '\n' };
	Info new_info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&content_stream}, {"name", ""}});
	emit(new_info);
}

Chat* Chat::clone() const
{
	return new Chat(*this);
}

} // namespace http
} // namespace docwire
