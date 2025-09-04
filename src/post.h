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

#ifndef DOCWIRE_HTTP_POST_H
#define DOCWIRE_HTTP_POST_H

#include "chain_element.h"
#include "http_export.h"
#include <filesystem>
#include <map>

namespace docwire
{

struct DefaultFileName
{
	std::filesystem::path v;
	explicit DefaultFileName(const std::filesystem::path& p)
		: v(p) {}
};

namespace http
{

class DOCWIRE_HTTP_EXPORT Post : public ChainElement, public with_pimpl<Post>
{
public:
	Post(const std::string& url, const std::string& oauth2_bearer_token = "");
	Post(const std::string& url, const std::map<std::string, std::string> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token = "");

	/**
	 * @brief Executes transform for the given message; consumes msg.
	 * @see docwire::message_ptr
	 * @see docwire::message_callbacks
	 * @param msg Message to process.
	 * @param emit_message Emission callbacks for produced messages.
	 */
	virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	using with_pimpl<Post>::impl;
};

} // namespace http
} // namespace docwire

#endif //DOCWIRE_HTTP_POST_H
