/// [example_cpp]
#include <algorithm>
#include <iostream>
#include <memory>

#include "parser.h"
#include "parser_builder.h"
#include "plain_text_writer.h"

/**
 * @example example_5.cpp
 */
int main(int argc, char* argv[])
{
  using namespace docwire;
  ParserManager parser_manager; // Create parser manager (load parsers)
  std::string path = argv[1];
  auto parser_builder = parser_manager.findParserByExtension(path); // get the parser builder by extension
  auto plain_text_writer = std::make_shared<PlainTextWriter>(); // create a plain text writer

  Info open_tag(StandardTag::TAG_DOCUMENT);
  plain_text_writer->write_to(open_tag, std::cout);
  if (parser_builder) // if parser builder exists
  {
    (*parser_builder)->build(path) // build the parser
            ->addOnNewNodeCallback([&plain_text_writer](Info &info) // add a callback function
                                   {
                                     plain_text_writer->write_to(info, std::cout); // write the node to the output stream
                                   })
                     .parse(); // start the parsing process
  }
  Info close_tag(StandardTag::TAG_CLOSE_DOCUMENT);
  plain_text_writer->write_to(close_tag, std::cout);
  return 0;
}
/// [example_cpp]
