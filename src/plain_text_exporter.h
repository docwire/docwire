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

#ifndef DOCWIRE_PLAIN_TEXT_EXPORTER_H
#define DOCWIRE_PLAIN_TEXT_EXPORTER_H

#include "chain_element.h"
#include <functional>
#include <memory>
#include <sstream>
#include "document_elements.h"
#include "error_tags.h"
#include "plain_text/output_width.h"
#include "plain_text_writer.h"
#include "data_source.h"
#include "throw_if.h"

namespace docwire
{
using docwire::plain_text::output_width;

struct eol_sequence { std::string v; };

struct link_formatter
{
	std::function<std::string(const document::link&)> format_opening;
	std::function<std::string(const document::close_link&)> format_closing;
};

/**
 * @brief Exports data to plain text format.
 */
class plain_text_exporter : public chain_element
{
public:
	plain_text_exporter(eol_sequence eol = eol_sequence{"\n"},
	                   link_formatter formatter = default_link_formatter,
	                   output_width max_output_width = output_width{80})
	  : m_writer{eol.v, formatter.format_opening, formatter.format_closing, max_output_width}
	{}

	virtual continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	inline static const link_formatter default_link_formatter =
	{
		.format_opening = [](const document::link& link)
		{
			return link.url ? "<" + *link.url + ">" : "";
		},
		.format_closing = [](const document::close_link&)
		{
			return "";
		}
	};

	std::shared_ptr<std::stringstream> m_stream;
	plain_text_writer m_writer;
	int m_nested_docs_level{0};
};

inline continuation plain_text_exporter::operator()(message_ptr msg, const message_callbacks& emit_message)
{
	if (msg->is<std::exception_ptr>())
		return emit_message(std::move(msg));
	if (msg->is<document::document>() || !m_stream)
	{
		++m_nested_docs_level;
		if (m_nested_docs_level == 1)
			m_stream = std::make_shared<std::stringstream>();
	}
	m_writer.write_to(msg, *m_stream);
	if (msg->is<document::close_document>())
	{
		DOCWIRE_THROW_IF(m_nested_docs_level <= 0, errors::program_logic{});
		--m_nested_docs_level;
		if (m_nested_docs_level == 0)
		{
			emit_message(data_source{seekable_stream_ptr{m_stream}, mime_type{"text/plain"}, confidence::highest});
			m_stream.reset();
		}
	}
	return continuation::proceed;
}

} // namespace docwire

#endif //DOCWIRE_PLAIN_TEXT_EXPORTER_H
