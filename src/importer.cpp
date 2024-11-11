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

#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"
#include "detect_by_file_extension.h"
#include "detect_by_signature.h"
#include "error_tags.h"
#include <fstream>
#include <filesystem>
#include <boost/signals2.hpp>

#include "exception_utils.h"
#include "importer.h"
#include "log.h"
#include "throw_if.h"

namespace docwire
{

class Importer::Implementation
{
public:
  Implementation(const ParserParameters &parameters, Importer& owner)
    : m_parameters(parameters),
      m_owner(owner)
  {}

  Implementation(const Implementation &other, Importer& owner)
    : m_parameters(other.m_parameters),
      m_owner(owner)
  {}

  Implementation(const Implementation &&other, Importer& owner)
    : m_parameters(other.m_parameters),
      m_owner(owner)
  {}

  bool
  isReadable(const std::filesystem::path& p) const
  {
    std::error_code ec;
    auto perms = std::filesystem::status(p, ec).permissions();
    if ((perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none &&
        (perms & std::filesystem::perms::group_read) != std::filesystem::perms::none &&
        (perms & std::filesystem::perms::others_read) != std::filesystem::perms::none
            )
    {
      return true;
    }
    return false;
  }

  std::unique_ptr<ParserBuilder> findParser(data_source& data) const
  {
    detect::by_file_extension(data);
    detect::by_signature(data);
    content_type::html::detect(data);
    content_type::iwork::detect(data);
    content_type::xlsb::detect(data);
    content_type::odf_flat::detect(data);
    content_type::outlook::detect(data);
    std::optional<mime_type> mt = data.highest_confidence_mime_type();
    throw_if(!mt, "MIME type detection failed for the data source", errors::uninterpretable_data{});
    throw_if(data.mime_type_confidence(mime_type { "application/encrypted" }) >= confidence { 50 }, errors::file_encrypted{});
    std::unique_ptr<ParserBuilder> builder = m_owner.find_parser_by_mime_type(*mt);
    throw_if (!builder, "find_parser_by_mime_type() failed", mt->v, errors::uninterpretable_data{});
    return builder;
  }

  std::unique_ptr<docwire::Parser> build_parser(ParserBuilder& builder)
  {
    return builder
        .withParameters(m_parameters)
        .build();
  }
    
  void
  process(Info& info)
  {
    if (!std::holds_alternative<data_source>(info.tag))
    {
      m_owner.emit(info);
      return;
    }
    auto data = std::get<data_source>(info.tag);
    std::unique_ptr<ParserBuilder> builder = findParser(data);
    std::unique_ptr<docwire::Parser> parser = build_parser(*builder);
    auto parser_callback = [this](const Tag& tag)
    {
      Info info{tag};
      process(info);
      if (info.cancel)
        return Parser::parsing_continuation::stop;
      else if (info.skip)
        return Parser::parsing_continuation::skip;
      else
        return Parser::parsing_continuation::proceed;
    }; 
    try
    {
      (*parser)(data, parser_callback);
    }
    catch (const std::exception& ex)
    {
      std::throw_with_nested(make_error("Parsing failed")));
    }
  }

  void
  add_parameters(const ParserParameters &parameters)
  {
    m_parameters += parameters;
  }

  std::shared_ptr<ParserBuilder> m_parser_builder;

  ParserParameters m_parameters;
  Importer& m_owner;
};

Importer::Importer(const ParserParameters &parameters)
{
  impl = std::unique_ptr<Implementation>{new Implementation{parameters, *this}};
}

Importer::Importer(const Importer &other)
  :  ChainElement(other),
     impl(new Implementation(*(other.impl), *this))
{}

Importer::Importer(const Importer &&other)
  :  ChainElement(other),
     impl(new Implementation(*(other.impl), *this))
{}

Importer::~Importer()
{
}

Importer&
Importer::operator=(const Importer &other)
{
  impl.reset(new Implementation(*(other.impl), *this));
  return *this;
}

Importer&
Importer::operator=(const Importer &&other)
{
  impl.reset(new Implementation(*(other.impl), *this));
  return *this;
}

void
Importer::process(Info& info) const
{
  impl->process(info);
}

void
Importer::add_parameters(const ParserParameters &parameters)
{
  impl->add_parameters(parameters);
}

} // namespace docwire
