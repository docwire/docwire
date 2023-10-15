#include <iostream>
#include <memory>
#include <optional>

#include "parser.h"
#include "parser_builder.h"
#include "parser_provider.h"
#include "simple_extractor.h"

using namespace docwire;

/// [plugin_example_1]

/**
 * @example example_9.cpp
 */

class CustomParser : public Parser
{
  void parse() const override
  {
    // parsing process (see Parser)
  }

  virtual Parser &addOnNewNodeCallback(NewNodeCallback callback) override
  {
    // manage callbacks (see Parser)
    return *this;
  }
};

class CustomParserBuilder : public ParserBuilder
{
  std::unique_ptr<Parser> build(const std::string &inFileName) const override
  {
    // build new parser from file (see ParserBuilder)
  }

  std::unique_ptr<Parser> build(const char *buffer, size_t size) const override
  {
    // build new parser from data buffer (see ParserBuilder)
  }

};

/// [plugin_example_1]

/// [plugin_example_2]

class CustomParserProvider : public ParserProvider
{
  std::optional<ParserBuilder*> findParserByExtension(const std::string &extension) const override
  {
      return std::nullopt;
  }

  std::optional<ParserBuilder*> findParserByData(const std::vector<char>& buffer) const override
  {
    return std::nullopt;
  }

  std::set<std::string> getAvailableExtensions() const override
  {
    return {".custom"};
  }

};

extern "C" DllExport CustomParserProvider custom_parser_provider;
CustomParserProvider custom_parser_provider;

/// [plugin_example_2]

/// [plugin_example_3]

int main(int argc, char* argv[])
{
  SimpleExtractor extractor("file.custom", "path_tp_directory_with_plugins");
  std::cout << extractor.getPlainText();
  return 0;
}

/// [plugin_example_3]
