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


#ifndef DOCWIRE_PARSER_PROVIDER_H
#define DOCWIRE_PARSER_PROVIDER_H

#include <optional>

#include <set>
#include "parser_builder.h"
#include "parser_wrapper.h"
#include "defines.h"

namespace docwire
{

/**
 * @brief The ParserProvider class.
 */
class DllExport ParserProvider
{
public:
  /**
   * @brief Returns parser builder for given mime type or nullopt if no parser is found.
   * @param mime mime type (e.g. "text/plain", "application/pdf", etc.)
   * @return unique_ptr to specific parser builder or null unique_ptr if no parser is found
   */
  virtual std::unique_ptr<ParserBuilder> find_parser_by_mime_type(const mime_type& mime) const = 0;

  /**
   * @brief Returns parser builder for given raw data or nullopt if no parser is found.
   * @param data_source raw data
   * @return unique_ptr to specific parser builder or null unique_ptr if no parser is found
   */
  virtual std::unique_ptr<ParserBuilder> findParserByData(const data_source& data) const = 0;

  virtual ~ParserProvider() = default;
};

template<typename T>
concept universal_parser_type = requires { { T::is_universal } -> std::convertible_to<bool>; } && T::is_universal;

template<typename... ParserTypes>
class parser_provider : public ParserProvider
{
public:

  parser_provider()
  {
  }

  std::unique_ptr<ParserBuilder> find_parser_by_mime_type(const mime_type& mime) const override
  {
    return find_parser_by_mime_type<ParserTypes...>(mime);
  }

  std::unique_ptr<ParserBuilder> findParserByData(const data_source& data) const override
  {
    return findParserByData<ParserTypes...>(data);
  }

private:

  bool is_mime_type_in_vector(const mime_type& mime, const std::vector<mime_type>& mime_type_list) const
  {
    return std::find(mime_type_list.begin(), mime_type_list.end(), mime) != mime_type_list.end();
  }

  template<typename T>
  bool parser_handle_mime_type(const mime_type& mime) const
  {
    return is_mime_type_in_vector(mime, T::supported_mime_types);
  }

  template <universal_parser_type T>
  bool parser_understands(const data_source& data) const
  {
    return false;
  }

  template <typename T>
  bool parser_understands(const data_source& data) const
  {
    T parser;
    return parser.understands(data);
  }

  template<typename T, typename... Ts>
  std::unique_ptr<ParserBuilder> find_parser_by_mime_type(const mime_type& mime) const
  {
    if (parser_handle_mime_type<T>(mime))
      return std::make_unique<ParserBuilderWrapper<T>>();
    if constexpr (sizeof...(Ts) > 0)
      return find_parser_by_mime_type<Ts...>(mime);
    return nullptr;
  }

  template<typename T, typename... Ts>
  std::unique_ptr<ParserBuilder> findParserByData(const data_source& data) const
  {
    if (parser_understands<T>(data))
      return std::make_unique<ParserBuilderWrapper<T>>();
    if constexpr (sizeof...(Ts) > 0)
      return findParserByData<Ts...>(data);
    return nullptr;
  }
};

} // namespace docwire

#endif //DOCWIRE_PARSER_PROVIDER_H
