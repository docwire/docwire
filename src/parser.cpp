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
#include "throw_if.h"

namespace docwire
{

continuation Parser::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
  if (!std::holds_alternative<data_source>(tag))
    return emit_tag(std::move(tag));
  auto data = std::get<data_source>(tag);
  std::optional<mime_type> mt = data.highest_confidence_mime_type();
  throw_if(!mt, "Data source has no mime type", errors::uninterpretable_data{});
  throw_if(data.mime_type_confidence(mime_type { "application/encrypted" }) >= confidence::high, errors::file_encrypted{});
  const std::vector<mime_type>& supported_mimes = supported_mime_types();
  if (std::find(supported_mimes.begin(), supported_mimes.end(), *mt) == supported_mimes.end())
    return emit_tag(std::move(tag));
  try
  {
      parse(data, emit_tag);
  }
  catch (const std::exception& e)
  {
    std::throw_with_nested(make_error("Parsing failed"));
  }
  return continuation::proceed;
}

} // namespace docwire
