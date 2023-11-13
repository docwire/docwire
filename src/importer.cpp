/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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
  Implementation(const std::shared_ptr<ParserManager> &parser_manager,
                 const ParserParameters &parameters, Importer& owner)
    : m_parser_manager(parser_manager),
      m_parameters(parameters),
      m_owner(owner)
  {}

  Implementation(const Implementation &other, Importer& owner)
    : m_parser_manager(other.m_parser_manager),
      m_parameters(other.m_parameters),
      m_owner(owner)
  {}

  Implementation(const Implementation &&other, Importer& owner)
    : m_parser_manager(other.m_parser_manager),
      m_parameters(other.m_parameters),
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
    if (info.tag_name != StandardTag::TAG_FILE)
    {
      m_owner.emit(info);
      return;
    }
    Info new_doc(StandardTag::TAG_DOCUMENT);
    m_owner.emit(new_doc);
    std::shared_ptr<ParserBuilder> builder;
    std::vector<char> buffer;
    std::istream* input_stream = nullptr;
    std::string file_path;
    if (info.getAttributeValue<std::istream*>("stream"))
    {
      input_stream = *info.getAttributeValue<std::istream*>("stream");
    }
    else if(info.getAttributeValue<std::string>("path"))
    {
      file_path = *info.getAttributeValue<std::string>("path");
    }
    if (!file_path.empty())
    {
      std::filesystem::path path{file_path};
      if (std::filesystem::exists(path))
      {
        if (isReadable(path))
        {
          builder = std::shared_ptr<ParserBuilder>(m_parser_manager->findParserByExtension(file_path).value_or(nullptr));
        }
        else
        {
          throw FileNotReadable("file " + file_path + " is not readable");
        }
      }
      else
      {
        throw FileNotFound("file " + file_path + "  doesn't exist");
      }
    }
    else if(input_stream)
    {
      buffer = std::vector<char>((std::istreambuf_iterator<char>(*input_stream)), std::istreambuf_iterator<char>());
      builder = std::shared_ptr<ParserBuilder>(m_parser_manager->findParserByData(buffer).value_or(nullptr));
    }
    if (builder)
    {
      auto &builder_ref = builder->withOnNewNodeCallbacks({[this](Info &info){ m_owner.emit(info);}})
        .withParserManager(m_parser_manager)
        .withParameters(m_parameters);

      if (!file_path.empty())
      {
        try
        {
          builder_ref.build(file_path)->parse();
        }
        catch (EncryptedFileException &ex)
        {
          throw ParsingFailed("Parsing failed, file is encrypted", ex);
        }
        catch (const std::exception& ex)
        {
          docwire_log(severity_level::info) << "It is possible that wrong parser was selected. Trying different parsers.";
          std::vector<char> buffer;
          load_file_to_buffer(file_path, buffer);
          auto second_builder = m_parser_manager->findParserByData(buffer);
          if (!second_builder)
          {
            throw ParsingFailed("Error parsing file: " + file_path  + ". Tried different parsers, but file could not be recognized as another format. File may be corrupted or encrypted", ex);
          }
          std::shared_ptr<ParserBuilder>(*second_builder)
                  ->withOnNewNodeCallbacks({[this](Info &info){ m_owner.emit(info);}})
                  .withParserManager(m_parser_manager)
                  .withParameters(m_parameters)
                  .build(buffer.data(), buffer.size())->parse();
        }
      }
      else
      {
        builder_ref.build(buffer.data(), buffer.size())->parse();
      }
    }
    else
    {
      throw UnknownFormat("File format was not recognized.");
    }
    Info end_doc(StandardTag::TAG_CLOSE_DOCUMENT);
    m_owner.emit(end_doc);
  }

  void
  add_parameters(const ParserParameters &parameters)
  {
    m_parameters += parameters;
  }

  std::shared_ptr<ParserManager> m_parser_manager;
  std::shared_ptr<ParserBuilder> m_parser_builder;

  ParserParameters m_parameters;
  Importer& m_owner;
};

Importer::Importer(const ParserParameters &parameters, const std::shared_ptr<ParserManager> &parser_manager)
{
  impl = std::unique_ptr<Implementation>{new Implementation{parser_manager, parameters, *this}};
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

Importer*
Importer::clone() const
{
  return new Importer(*this);
}

void
Importer::add_parameters(const ParserParameters &parameters)
{
  impl->add_parameters(parameters);
}

} // namespace docwire
