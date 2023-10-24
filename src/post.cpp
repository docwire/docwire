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

#include "post.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "exception.h"
#include <fstream>
#include "log.h"
#include "parser.h"
#include <sstream>
#include "version.h"

namespace docwire
{
namespace http
{

struct Post::Implementation
{
	std::string m_url;
	Implementation(const std::string& url)
		: m_url(url)
	{}
};

Post::Post(const std::string& url)
	: impl(new Implementation{url})
{
}

Post::Post(const Post &other)
	: impl(new Implementation(*other.impl))
{
	docwire_log_func();
}

Post::~Post()
{
}

void
Post::process(Info &info) const
{
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	docwire_log(debug) << "TAG_FILE received";
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	if(!path && !stream)
		throw Exception("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;

	curlpp::Easy request;
	request.setOpt<curlpp::options::Url>(impl->m_url);
	request.setOpt(curlpp::options::UserAgent(std::string("DocWire SDK/") + VERSION));
	request.setOpt(new curlpp::options::CustomRequest("POST"));
	request.setOpt(curlpp::options::ReadFunction([in_stream](char* buf, size_t size, size_t nitems) -> size_t
	{
		docwire_log_func_with_args(size, nitems);
		in_stream->read(buf, static_cast<std::streamsize>(size * nitems));
		return in_stream->gcount();
	}));
	request.setOpt<curlpp::options::Encoding>("gzip");
	request.setOpt(curlpp::options::Upload(true));
	std::stringstream response_stream;
	curlpp::options::WriteStream ws(&response_stream);
	request.setOpt(ws);
	try
	{
		request.perform();
	}
	catch (curlpp::LogicError &e)
	{
		throw Exception(e.what());
    }
	catch (curlpp::RuntimeError &e)
	{
		throw Exception(e.what());
	}
	Info new_info(StandardTag::TAG_FILE, "", {{"stream", (std::istream*)&response_stream}, {"name", ""}});
	emit(new_info);
	if (path)
		delete in_stream;
}

Post* Post::clone() const
{
	return new Post(*this);
}

} // namespace http
} // namespace docwire
