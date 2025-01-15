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

#ifndef DOCWIRE_PARSING_CHAIN_ADAPTERS_H
#define DOCWIRE_PARSING_CHAIN_ADAPTERS_H

#include "chain_element.h"
#include "data_source.h"
#include "error_tags.h"
#include <functional>
#include "parsing_chain.h"
#include "ref_or_owned.h"
#include "throw_if.h"
#include "transformer_func.h"

namespace docwire
{

inline ParsingChain operator|(ref_or_owned<ChainElement> element, NewNodeCallback func)
{
  return element | TransformerFunc{func};
}

template<typename T>
concept parser_derived = std::derived_from<T, Parser>;

template<typename T>
concept parser_derived_ref_qualified = parser_derived<std::remove_reference_t<T>>;

inline bool is_mime_type_in_vector(const mime_type& mime, const std::vector<mime_type>& mime_type_list)
  {
    return std::find(mime_type_list.begin(), mime_type_list.end(), mime) != mime_type_list.end();
  }

template<typename T>
bool parser_handle_mime_type(const mime_type& mime)
{
  return is_mime_type_in_vector(mime, T::supported_mime_types);
}

template<parser_derived T>
class parser_chain_element : public ChainElement
{
public:
  explicit parser_chain_element(ref_or_owned<T> parser) : m_parser(parser) {}
  void process(Info &info) override
  {
    if (!std::holds_alternative<data_source>(info.tag))
    {
      emit(info);
      return;
    }
    auto data = std::get<data_source>(info.tag);
    std::optional<mime_type> mt = data.highest_confidence_mime_type();
    DOCWIRE_THROW_IF(!mt, "Data source has no mime type", errors::uninterpretable_data{});
    DOCWIRE_THROW_IF(data.mime_type_confidence(mime_type { "application/encrypted" }) >= confidence::high, errors::file_encrypted{});
    if (!parser_handle_mime_type<T>(*mt))
    {
      emit(info);
      return;
    }
    auto parser_callback = [this](const Tag& tag)
    {
      Info info{tag};
      if (std::holds_alternative<data_source>(tag))
      {
        std::optional<std::reference_wrapper<ParsingChain>> ch = chain();
        DOCWIRE_THROW_IF(!ch, "Cannot send datasource to top chain because chain is not assigned", errors::program_logic{});
        ch->get().top_chain().process(info);
      }
      else
        emit(info);
      if (info.cancel)
        return Parser::parsing_continuation::stop;
      else if (info.skip)
        return Parser::parsing_continuation::skip;
      else
        return Parser::parsing_continuation::proceed;
    };
    try
    {
      m_parser.get()(data, parser_callback);
    }
    catch (const std::exception& e)
    {
      std::throw_with_nested(DOCWIRE_MAKE_ERROR("Parsing failed"));
    }
  }
  bool is_leaf() const override
  {
    return false;
  }
private:
  ref_or_owned<T> m_parser;
};

template<parser_derived T>
inline ParsingChain operator|(ref_or_owned<ChainElement> element, T&& parser)
{
  return std::move(element | parser_chain_element<T>(std::forward<T>(parser)));
}

template<parser_derived T>
inline ParsingChain operator|(ref_or_owned<T> parser, ref_or_owned<ChainElement> element)
{
  return parser_chain_element<T>(parser) | element;
}

template<data_source_compatible_type_ref_qualified T, parser_derived_ref_qualified U>
inline ParsingChain operator|(T&& v, U&& parser)
{
  return std::forward<T>(v) | parser_chain_element<U>(std::forward<U>(parser));
}

template<parser_derived_ref_qualified L, parser_derived_ref_qualified R>
inline ParsingChain operator|(L&& lhs, R&& rhs)
{
  return parser_chain_element<L>(std::forward<L>(lhs)) | parser_chain_element<R>(std::forward<R>(rhs));
}

} // namespace docwire

#endif //DOCWIRE_PARSING_CHAIN_ADAPTERS_H
