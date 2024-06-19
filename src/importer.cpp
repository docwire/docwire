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

#include <fstream>
#include <filesystem>
#include <boost/signals2.hpp>

#include "exception.h"
#include "importer.h"
#include "log.h"

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

  std::unique_ptr<ParserBuilder> findParser(const data_source& data) const
  {
    std::optional<file_extension> extension = data.file_extension();
    if (!extension)
    {
      std::unique_ptr<ParserBuilder> builder = m_owner.findParserByData(data);
      if (!builder)
        throw UnknownFormat("Format of data in buffer was not recognized.");
      return std::move(builder);
    }
    else
    {
      std::unique_ptr<ParserBuilder> builder = m_owner.findParserByExtension(*extension);
      if (!builder)
        throw UnknownFormat("Format was not recognized by extension for extension " + extension->string());
      return std::move(builder);
    }
  }

  std::unique_ptr<docwire::Parser> build_parser(ParserBuilder& builder)
  {
    return builder.withOnNewNodeCallbacks({[this](Info &info){ process(info); }})
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
    try
    {
      parser->parse(data);
    }
    catch (EncryptedFileException &ex)
    {
      throw ParsingFailed("Parsing failed, file is encrypted", ex);
    }
    catch (const std::exception& ex)
    {
      if (!data.file_extension()) // parser was detected by data
        throw;
      docwire_log(severity_level::info) << "It is possible that wrong parser was selected. Trying different parsers.";
      auto second_builder = m_owner.findParserByData(data);
      if (!second_builder)
      {
        throw;
      }
      build_parser(*second_builder)->parse(data);
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
