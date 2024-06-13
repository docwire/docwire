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

#ifndef DOCWIRE_MAIL_PARSER_PROVIDER_H
#define DOCWIRE_MAIL_PARSER_PROVIDER_H


#include "parser_provider.h"
#include "defines.h"

namespace docwire
{

class DllExport MailParserProvider : public ParserProvider
{
public:
  MailParserProvider();
  std::unique_ptr<ParserBuilder> findParserByExtension(const file_extension& extension) const override;
  std::unique_ptr<ParserBuilder> findParserByData(const data_source& data) const override;
  std::set<file_extension> getAvailableExtensions() const override;

private:
  void addExtensions(const std::vector<file_extension> &inExtensions);
  bool isExtensionInVector(const file_extension& extension, const std::vector<file_extension>& extension_list) const;
  std::set<file_extension> available_extensions;
};

} // namespace docwire

#endif //DOCWIRE_MAIL_PARSER_PROVIDER_H
