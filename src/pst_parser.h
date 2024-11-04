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


#ifndef DOCWIRE_PST_PARSER_H
#define DOCWIRE_PST_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class DllExport PSTParser : public Parser
{
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;

public:

  void parse(const data_source& data) const override;

  inline static const std::vector<mime_type> supported_mime_types =
  {
    mime_type{"application/vnd.ms-outlook-pst"},
    mime_type{"application/vnd.ms-outlook-ost"}
  };

  PSTParser();
  ~PSTParser();
};

} // namespace docwire

#endif //DOCWIRE_PST_PARSER_H
