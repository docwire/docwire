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


#ifndef DOCWIRE_OFFICE_FORMATS_PARSER_H
#define DOCWIRE_OFFICE_FORMATS_PARSER_H

#include "doc_parser.h"
#include "html_parser.h"
#include "iwork_parser.h"
#include "odfxml_parser.h"
#include "pdf_parser.h"
#include "xls_parser.h"
#include "xlsb_parser.h"
#include "odf_ooxml_parser.h"
#include "parsing_chain.h"
#include "ppt_parser.h"
#include "rtf_parser.h"
#include "txt_parser.h"
#include "xml_parser.h"

namespace docwire
{

/**
 * @brief A composite parser handling various office document formats.
 * @tparam safety_level The safety policy to use.
 */
template <safety_policy safety_level = default_safety_level>
class office_formats_parser : public pipeline::transformer_element<office_formats_parser<safety_level>>
{
    public:
        /**
         * @brief Executes the composite chain of format parsers.
         */
        continuation operator()(message_ptr msg, const message_callbacks& emit_message)
        {
            return m_chain(msg, emit_message);
        }

    private:
        static auto make_chain()
        {
            return html_parser{} |
                doc_parser{} |
                pdf_parser{} |
                xls_parser{} |
                xlsb_parser{} |
                iwork_parser{} |
                ppt_parser{} |
                rtf_parser{} |
                odf_ooxml_parser<safety_level>{} |
                odfxml_parser<safety_level>{} |
                xml_parser<safety_level>{} |
                txt_parser{};
        }

        decltype(make_chain()) m_chain{make_chain()};
};

} // namespace docwire

#endif //DOCWIRE_OFFICE_FORMATS_PARSER_H
