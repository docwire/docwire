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

#ifndef DOCWIRE_FORMATTING_STYLE_H
#define DOCWIRE_FORMATTING_STYLE_H

#include <string>
#include "defines.h"

namespace docwire
{
	enum class TableStyle { table_look, one_row, one_col };
	enum class UrlStyle { text_only, extended, underscored };

	class DllExport ListStyle
	{
		private:
			struct Implementation;
			Implementation* m_impl;

		public:
			ListStyle();
			~ListStyle();
			ListStyle(const ListStyle& style);
			ListStyle& operator = (const ListStyle& style);
			///Sets leading characters for each position in the list. prefix must be a value encoded in UTF8. Default value: empty string (no prefix)
			void setPrefix(const std::string& prefix);
			void setPrefix(const char* prefix);
			const char* getPrefix() const;
	};

	struct FormattingStyle
	{
		FormattingStyle()
		: table_style(TableStyle::table_look), url_style(UrlStyle::extended)
		{}
		FormattingStyle(const TableStyle &tableStyle, const UrlStyle &urlStyle)
			: table_style(tableStyle), url_style(urlStyle)
		{}
		TableStyle table_style;
		UrlStyle url_style;
		ListStyle list_style;
	};

	enum XmlParseMode { PARSE_XML, FIX_XML, STRIP_XML };
} // namespace docwire

#endif
