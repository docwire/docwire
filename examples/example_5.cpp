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
  doctotext::ParserManager parser_manager; // Create parser manager (load parsers)
  std::string path = argv[1];
  auto parser_builder = parser_manager.findParserByExtension(path); // get the parser builder by extension
  auto plain_text_writer = std::make_shared<doctotext::PlainTextWriter>(); // create a plain text writer
  plain_text_writer->write_header(std::cout); // write the header to the output stream
  if (parser_builder) // if parser builder exists
  {
    (*parser_builder)->build(path) // build the parser
            ->addOnNewNodeCallback([&plain_text_writer](doctotext::Info &info) // add a callback function
                                   {
                                     plain_text_writer->write_to(info,
                                                                 std::cout); // write the node to the output stream
                                   })
                     .parse(); // start the parsing process
  }
  plain_text_writer->write_footer(std::cout); // write the footer to the output stream
  return 0;
}
/// [example_cpp]