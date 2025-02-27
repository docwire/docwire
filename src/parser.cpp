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

#include "parser.h"

#include "error_tags.h"
#include "log.h"
#include "log_tags.h" // IWYU pragma: keep
#include "parsing_chain.h"
#include "throw_if.h"
#include <iostream>

namespace docwire
{

void Parser::process(Info &info)
{
  if (!std::holds_alternative<data_source>(info.tag))
  {
    emit(info);
    return;
  }
  auto data = std::get<data_source>(info.tag);
  std::optional<mime_type> mt = data.highest_confidence_mime_type();
  throw_if(!mt, "Data source has no mime type", errors::uninterpretable_data{});
  throw_if(data.mime_type_confidence(mime_type { "application/encrypted" }) >= confidence::high, errors::file_encrypted{});
  const std::vector<mime_type>& supported_mimes = supported_mime_types();
  if (std::find(supported_mimes.begin(), supported_mimes.end(), *mt) == supported_mimes.end())
  {
    emit(info);
    return;
  }
  try
  {
      parse(data);
  }
  catch (const std::exception& e)
  {
    std::cerr << "parsing failed:\n" << errors::diagnostic_message(e) << std::endl;
    std::throw_with_nested(make_error("Parsing failed"));
  }
}

Info Parser::sendTag(const Tag& tag) const
{
  docwire_log_func_with_args(tag);
  Info info(tag);
  if (std::holds_alternative<data_source>(tag))
  {
    std::optional<std::reference_wrapper<ParsingChain>> ch = chain();
    throw_if(!ch, "Cannot send datasource to top chain because chain is not assigned", errors::program_logic{});
    ch->get().top_chain()(tag);
  }
  else
    emit(info);
  return info;
}

Info Parser::sendTag(const Info &info) const
{
  return sendTag(info.tag);
}

} // namespace docwire
