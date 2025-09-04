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

#ifndef DOCWIRE_ODFXML_PARSER_H
#define DOCWIRE_ODFXML_PARSER_H

#include "common_xml_document_parser.h"
#include "odf_ooxml_export.h"
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_ODF_OOXML_EXPORT ODFXMLParser : public CommonXMLDocumentParser, public with_pimpl<ODFXMLParser>
{
	private:
		using with_pimpl<ODFXMLParser>::impl;
		friend pimpl_impl<ODFXMLParser>;

	protected:
		CommonXMLDocumentParser::scoped_context_stack_push create_base_context_guard(const message_callbacks& emit_message)
		{
			return CommonXMLDocumentParser::scoped_context_stack_push{*this, emit_message};
		}

	public:
		ODFXMLParser();
		continuation operator()(message_ptr msg, const message_callbacks& emit_message) override;
		bool is_leaf() const override { return false; }
};

} // namespace docwire

#endif
