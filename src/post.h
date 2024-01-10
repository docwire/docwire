/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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
#include "exception.h"
#include <filesystem>

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

class DllExport Post : public ChainElement
{
public:
	Post(const std::string& url, const std::string& oauth2_bearer_token = "");
	Post(const std::string& url, const std::map<std::string, std::string> form, const std::string& pipe_field_name, const DefaultFileName& default_file_name, const std::string& oauth2_bearer_token = "");
	Post(const Post& other);
	virtual ~Post();

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Info
	* @param info
	*/
	void process(Info &info) const;

	bool is_leaf() const override
	{
		return false;
	}

	/**
	* @brief Creates clone of the Post
	* @return new Post
	*/
	Post* clone() const override;

	DOCWIRE_EXCEPTION_DEFINE(RequestFailed, RuntimeError);
	DOCWIRE_EXCEPTION_DEFINE(RequestIncorrect, LogicError);
	DOCWIRE_EXCEPTION_DEFINE(FileTagIncorrect, LogicError);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace http
} // namespace docwire

#endif //DOCWIRE_HTTP_POST_H
