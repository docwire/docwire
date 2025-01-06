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

#include "log.h"
#include "log_tags.h" // IWYU pragma: keep

namespace docwire
{

template<>
struct pimpl_impl<Parser> : pimpl_impl_base
{
  using parsing_continuation = Parser::parsing_continuation;
  Info sendTag(const Tag& tag) const
  {
    parsing_continuation continuation = m_callback(tag);
    Info info(tag);
    info.skip = continuation == parsing_continuation::skip;
    info.cancel = continuation == parsing_continuation::stop;
    return info;
  }

  mutable std::function<parsing_continuation(const Tag&)> m_callback;
};

Parser::Parser()
{
}

Info Parser::sendTag(const Tag& tag) const
{
  docwire_log_func_with_args(tag);
  return impl().sendTag(tag);
}

Info Parser::sendTag(const Info &info) const
{
  return impl().sendTag(info.tag);
}

void Parser::operator()(const data_source& data, std::function<parsing_continuation(const Tag&)> callback)
{
  impl().m_callback = callback;
  parse(data);
}

} // namespace docwire
