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

#include <vector>

#include "parser.h"

namespace docwire
{

class DllExport PSTParser : public Parser
{
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;

public:

  void parse(const data_source& data) const override;
  static std::vector<file_extension> getExtensions()
  {
    return { file_extension{".pst"}, file_extension{".ost"} };
  }

  PSTParser();
  ~PSTParser();
  bool understands(const data_source& data) const override;
};

} // namespace docwire

#endif //DOCWIRE_PST_PARSER_H
