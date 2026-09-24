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

#ifndef DOCWIRE_MAIL_PARSER_H
#define DOCWIRE_MAIL_PARSER_H

#include "eml_parser.h"
#include "parsing_chain.h"
#include "pst_parser.h"

namespace docwire
{

class mail_parser : public pipeline::transformer_element<mail_parser>
{
    public:
        continuation operator()(message_ptr msg, const message_callbacks& emit_message)
        {
            return m_chain(msg, emit_message);
        }

    private:
        parsing_chain<eml_parser, pst_parser> m_chain{eml_parser{}, pst_parser{}};
};

} // namespace docwire

#endif //DOCWIRE_MAIL_PARSER_H
