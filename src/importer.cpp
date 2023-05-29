/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include <fstream>
#include <filesystem>
#include <boost/signals2.hpp>

#include "exception.h"
#include "importer.h"

using namespace doctotext;

class Importer::Implementation
{
public:
  Implementation(const std::shared_ptr<ParserManager> &parser_manager,
                 const ParserParameters &parameters)
    : m_parser_manager(parser_manager),
      m_parameters(parameters)
  {}

  Implementation(const std::shared_ptr<ParserManager> &parser_manager,
                 const std::string &file_name,
                 const ParserParameters &parameters)
    : m_parser_manager(parser_manager),
      m_file_name(file_name),
      m_parameters(parameters)
  {}

  Implementation(const std::shared_ptr<ParserManager> &parser_manager,
                 std::istream &input_stream,
                 const ParserParameters &parameters)
    : m_parser_manager(parser_manager),
      m_parameters(parameters),
      m_input_stream(&input_stream)
  {}

  Implementation(const Implementation &other)
    : m_parser_manager(other.m_parser_manager),
      m_file_name(other.m_file_name),
      m_parameters(other.m_parameters),
      m_input_stream(other.m_input_stream)
  {}

  Implementation(const Implementation &&other)
    : m_parser_manager(other.m_parser_manager),
      m_file_name(other.m_file_name),
      m_parameters(other.m_parameters),
      m_input_stream(other.m_input_stream)
  {}

  void
  set_input_stream(std::istream &input_stream)
  {
    m_input_stream = &input_stream;
    m_file_name.clear();
  }

  bool is_valid() const
  {
    return !m_file_name.empty() || m_input_stream != nullptr;
  }

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

	std::ostream&
	getLogOutStream() const
	{
		auto log_stream = m_parameters.getParameterValue<std::ostream*>("log_stream");
		if (log_stream)
		{
			return *(*log_stream);
		}
		return std::cerr;
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
  process() const
  {
		auto log_stream = &getLogOutStream();
    std::shared_ptr<ParserBuilder> builder;
    std::vector<char> buffer;
    if (!m_file_name.empty())
    {
      std::filesystem::path path{m_file_name};
      if (std::filesystem::exists(path))
      {
        if (isReadable(path))
        {
          builder = std::shared_ptr<ParserBuilder>(m_parser_manager->findParserByExtension(m_file_name).value_or(nullptr));
        }
        else
        {
          throw doctotext::Exception("file " + m_file_name + " is not readable");
        }
      }
      else
      {
        throw doctotext::Exception("file " + m_file_name + "  doesn't exist");
      }
    }
    else if(m_input_stream)
    {
      buffer = std::vector<char>((std::istreambuf_iterator<char>(*m_input_stream)), std::istreambuf_iterator<char>());
      builder = std::shared_ptr<ParserBuilder>(m_parser_manager->findParserByData(buffer).value_or(nullptr));
    }
    if (builder)
    {
      auto &builder_ref = builder->withOnNewNodeCallbacks({[this](doctotext::Info &info){m_on_new_node_signal(info);}})
        .withParserManager(m_parser_manager)
        .withParameters(m_parameters);

      if (!m_file_name.empty())
      {
        try
        {
          builder_ref.build(m_file_name)->parse();
        }
        catch (doctotext::EncryptedFileException &ex)
        {
          throw ex;
        }
        catch (doctotext::Exception &ex)
        {
					(*log_stream) << "It is possible that wrong parser was selected. Trying different parsers." << std::endl;
          std::vector<char> buffer;
          load_file_to_buffer(m_file_name, buffer);
          auto second_builder = m_parser_manager->findParserByData(buffer);
          if (!second_builder)
          {
            ex.appendError("Error parsing file: " + m_file_name  + ". Tried different parsers, but file could not be recognized as another format. File may be corrupted or encrypted");
            throw ex;
          }
          std::shared_ptr<ParserBuilder>(*second_builder)
                  ->withOnNewNodeCallbacks({[this](doctotext::Info &info){m_on_new_node_signal(info);}})
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
      throw doctotext::Exception("File format was not recognized.");
    }
  }

  void
  add_parameters(const ParserParameters &parameters)
  {
    m_parameters += parameters;
  }

  void
  add_callback(NewNodeCallback callback)
  {
    m_on_new_node_signal.connect(callback);
  }

  void
  disconnect_all()
  {
    m_on_new_node_signal.disconnect_all_slots();
  }

  std::shared_ptr<ParserManager> m_parser_manager;
  std::shared_ptr<ParserBuilder> m_parser_builder;

  boost::signals2::signal<void(doctotext::Info &info)> m_on_new_node_signal;
  std::string m_file_name;
  std::istream *m_input_stream;
  ParserParameters m_parameters;
};

Importer::Importer(const ParserParameters &parameters, const std::shared_ptr<ParserManager> &parser_manager)
{
  impl = std::unique_ptr<Implementation>{new Implementation{parser_manager, parameters}};
}

Importer::Importer(const std::string &file_name,
                   const ParserParameters &parameters,
                   const std::shared_ptr<ParserManager> &parser_manager)
{
  impl = std::unique_ptr<Implementation>{new Implementation{parser_manager, file_name, parameters}};
}

Importer::Importer(std::istream &input_stream,
                   const ParserParameters &parameters,
                   const std::shared_ptr<ParserManager> &parser_manager)
{
  impl = std::unique_ptr<Implementation>{new Implementation{parser_manager, input_stream, parameters}};
}

Importer::Importer(const Importer &other)
 : impl(new Implementation(*(other.impl)))
{}

Importer::~Importer()
{
}

Importer&
Importer::operator=(const Importer &other)
{
  impl.reset(new Implementation(*(other.impl)));
  return *this;
}

void
Importer::set_input_stream(std::istream &input_stream)
{
  impl->set_input_stream(input_stream);
}

bool
Importer::is_valid() const
{
  return impl->is_valid();
}

void
Importer::process() const
{
  impl->process();
}

void
Importer::add_callback(const NewNodeCallback &callback)
{
  impl->add_callback(callback);
}

void
Importer::add_parameters(const ParserParameters &parameters)
{
  impl->add_parameters(parameters);
}

void
Importer::disconnect_all()
{
  impl->disconnect_all();
}