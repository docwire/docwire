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

#ifndef DOCWIRE_MAIL_PARSER_H
#define DOCWIRE_MAIL_PARSER_H

#include "eml_parser.h"
#include "parsing_chain_adapters.h"
#include "pst_parser.h"

namespace docwire
{

class mail_parser : public ParsingChain
{
    public:
        mail_parser()
            : ParsingChain{EMLParser{} | PSTParser{}}
        {}
};

} // namespace docwire

#endif //DOCWIRE_MAIL_PARSER_H
