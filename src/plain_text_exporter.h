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

#ifndef DOCWIRE_PLAIN_TEXT_EXPORTER_H
#define DOCWIRE_PLAIN_TEXT_EXPORTER_H

#include "chain_element.h"
#include "document_elements.h"

namespace docwire
{

struct eol_sequence { std::string v; };

struct link_formatter
{
	std::function<std::string(const document::Link&)> format_opening;
	std::function<std::string(const document::CloseLink&)> format_closing;
};

/**
 * @brief Exports data to plain text format.
 */
class DOCWIRE_CORE_EXPORT PlainTextExporter: public ChainElement, public with_pimpl<PlainTextExporter>
{
public:
	PlainTextExporter(eol_sequence eol = eol_sequence{"\n"}, link_formatter formatter = default_link_formatter);

	virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	inline static const link_formatter default_link_formatter =
	{
		.format_opening = [](const document::Link& link)
		{
			return link.url ? "<" + *link.url + ">" : "";
		},
		.format_closing = [](const document::CloseLink&)
		{
			return "";
		}
	};

	using with_pimpl<PlainTextExporter>::impl;
};

} // namespace docwire

#endif //DOCWIRE_PLAIN_TEXT_EXPORTER_H
