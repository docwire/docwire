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

  bool load_file_to_buffer(const std::string &file_name, std::vector<char> &buffer) const
  {
    std::ifstream src(file_name.c_str(), std::ios_base::in|std::ios_base::binary);
    if (!src.is_open())
    {
      return false;
    }
    buffer = std::vector<char>(std::istreambuf_iterator<char>(src), std::istreambuf_iterator<char>());
    return true;
  }

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

  void
  process(Info& info)
  {
    if (!std::holds_alternative<tag::File>(info.tag))
    {
      m_owner.emit(info);
      return;
    }
    Info new_doc(tag::Document{});
    m_owner.emit(new_doc);
    auto file = std::get<tag::File>(info.tag);
    if (std::holds_alternative<std::filesystem::path>(file.source))
    {
      std::filesystem::path file_path = std::get<std::filesystem::path>(file.source);
      if (!std::filesystem::exists(file_path))
        throw FileNotFound("file " + file_path.string() + "  doesn't exist");
      if (!isReadable(file_path))
        throw FileNotReadable("file " + file_path.string() + " is not readable");
      std::unique_ptr<ParserBuilder> builder = m_owner.findParserByExtension(file_path.string());
      if (!builder)
        throw UnknownFormat("File format was not recognized.");
      auto &builder_ref = builder->withOnNewNodeCallbacks({[this](Info &info){ m_owner.emit(info);}})
        .withImporter(m_owner)
        .withParameters(m_parameters);
        try
        {
          builder_ref.build(file_path.string())->parse();
        }
        catch (EncryptedFileException &ex)
        {
          throw ParsingFailed("Parsing failed, file is encrypted", ex);
        }
        catch (const std::exception& ex)
        {
          docwire_log(severity_level::info) << "It is possible that wrong parser was selected. Trying different parsers.";
          std::vector<char> buffer;
          load_file_to_buffer(file_path.string(), buffer);
          auto second_builder = m_owner.findParserByData(buffer);
          if (!second_builder)
          {
            throw ParsingFailed("Error parsing file: " + file_path.string()  + ". Tried different parsers, but file could not be recognized as another format. File may be corrupted or encrypted", ex);
          }
          second_builder->withOnNewNodeCallbacks({[this](Info &info){ m_owner.emit(info);}})
                  .withImporter(m_owner)
                  .withParameters(m_parameters)
                  .build(buffer.data(), buffer.size())->parse();
        }
    }
    else if (std::holds_alternative<std::shared_ptr<std::istream>>(file.source))
    {
      std::shared_ptr<std::istream> input_stream = std::get<std::shared_ptr<std::istream>>(file.source);
      std::vector<char> buffer = std::vector<char>((std::istreambuf_iterator<char>(*input_stream)), std::istreambuf_iterator<char>());
      std::unique_ptr<ParserBuilder> builder = m_owner.findParserByData(buffer);
      if (!builder)
        throw UnknownFormat("File format was not recognized.");
      auto &builder_ref = builder->withOnNewNodeCallbacks({[this](Info &info){ m_owner.emit(info);}})
        .withImporter(m_owner)
        .withParameters(m_parameters);
      builder_ref.build(buffer.data(), buffer.size())->parse();
    }
    Info end_doc(tag::CloseDocument{});
    m_owner.emit(end_doc);
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
