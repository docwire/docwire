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

#include <algorithm>
#include <numeric>
#include <filesystem>
#include <boost/dll/import.hpp>
#include <iostream>

#include "log.h"
#include "misc.h"
#include "parser_manager.h"
#include "parser_provider.h"
#include "parser_builder.h"
#include "pthread.h"

namespace docwire
{

static pthread_mutex_t load_providers_mutex = PTHREAD_MUTEX_INITIALIZER;

static std::set<std::string> correct_extensions = {".dll", ".so", ".dylib"};

class ParserManager::Implementation
{
public:
  explicit Implementation(const std::string &plugins_directory)
  {
    m_plugins_directory = plugins_directory.empty() ?
      locate_subpath("docwire_plugins").string() : plugins_directory;
    docwire_log(debug) << "Plugins directory: " << m_plugins_directory;
    pthread_mutex_lock(&load_providers_mutex);
    loadProviders();
    pthread_mutex_unlock(&load_providers_mutex);
  }

  void loadProvider(const std::filesystem::path path)
  {
    docwire_log(debug) << "Loading plugin " << path;
    try
    {
      boost::shared_ptr<ParserProvider>plugin_provider =
      boost::dll::import_symbol<ParserProvider>(path.c_str(),
                                                              "plugin_parser_provider",
                                                              boost::dll::load_mode::append_decorations);
      providers.push_back(plugin_provider);
      docwire_log(debug) << "Plugin " << path << " loaded successfuly";
    }
    catch (const boost::system::system_error &e)
    {
      docwire_log(error) << "Error loading plugin: " << e.what();
    }
  }

  void loadProviders()
  {
    docwire_log(debug) << "Loading plugins";
    providers.clear();
    const std::filesystem::path root{m_plugins_directory};
    try
    {
      if (std::filesystem::is_directory(root))
      {
        for (auto &dir_entry : std::filesystem::directory_iterator{root})
          if (dir_entry.is_regular_file() && correct_extensions.find(dir_entry.path().extension().u8string()) != correct_extensions.end())
            loadProvider(dir_entry.path());
          else
            docwire_log(error) << "Skipping " << dir_entry.path() << " - not a regular file or incorrect extension";
      }
      else
        docwire_log(error) << "Plugins directory path does not point to a directory";
    }
    catch (const std::filesystem::filesystem_error& e)
    {
      docwire_log(error) << "Error traversing plugins directory: " << e.what();
    }
  }

  std::vector<boost::shared_ptr<ParserProvider>> providers;
  std::string m_plugins_directory;
};

ParserManager::ParserManager()
{
  impl = std::make_unique<Implementation>("");
}

ParserManager::ParserManager(const std::string &plugins_directory)
{
  impl = std::make_unique<Implementation>(plugins_directory);
}

ParserManager::~ParserManager()
{
}

std::optional<ParserBuilder*>
ParserManager::findParserByExtension(const std::string &file_name) const
{
  std::string extension = file_name.substr(file_name.find_last_of(".") + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  for (const auto &provider : impl->providers)
  {
    auto builder = provider->findParserByExtension(extension);
    if (builder)
    {
      return builder;
    }
  }
  return std::nullopt;
}

std::optional<ParserBuilder*>
ParserManager::findParserByData(const std::vector<char> &buffer) const
{
  for (const auto &provider : impl->providers)
  {
    auto builder = provider->findParserByData(buffer);
    if (builder)
    {
      return builder;
    }
  }
  return std::nullopt;
}

std::set<std::string>
ParserManager::getAvailableExtensions() const
{
  std::set<std::string> all_extensions;
  for (const auto &provider : impl->providers)
  {
    auto extensions = provider->getAvailableExtensions();
    std::copy(extensions.begin(), extensions.end(), std::inserter(all_extensions, all_extensions.end()));
  }
  return all_extensions;
}

} // namespace docwire
