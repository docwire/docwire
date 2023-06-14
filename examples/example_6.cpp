/// [example_cpp]
#include <algorithm>
#include <iostream>
#include <memory>

#include "parser.h"
#include "parser_builder.h"
#include "plain_text_writer.h"

/**
 * @example example_6.cpp
 */
int main(int argc, char* argv[])
{
  auto parser_manager = std::make_shared<doctotext::ParserManager>(); // Create parser manager (load parsers)
  std::string path = argv[1];
  auto parser_builder = parser_manager->findParserByExtension(path); // get the parser builder by extension
  doctotext::PlainTextWriter plain_text_writer; // create a plain text writer
  doctotext::Info open_tag(doctotext::StandardTag::TAG_DOCUMENT);
  plain_text_writer.write_to(open_tag, std::cout);
  if (parser_builder) // if parser builder exists
  {
    (*parser_builder)->withParserManager(parser_manager) // set the parser manager
            .build(path) // build the parser
            ->addOnNewNodeCallback([](doctotext::Info &info) // add a callback function to filter by subject text
                                   {
                                     if (info.tag_name ==
                                         doctotext::StandardTag::TAG_MAIL) // if current node is mail
                                     {
                                       auto subject = info.getAttributeValue<std::string>(
                                               "subject"); // get the subject attribute
                                       if (subject) // if subject attribute exists
                                       {
                                         if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
                                         {
                                           info.skip = true; // skip the current node
                                         }
                                       }
                                     }
                                   })
            .addOnNewNodeCallback([&plain_text_writer](
                    doctotext::Info &info) // add callback function to write the parsed text to the output stream
                                  {
                                    plain_text_writer.write_to(info, std::cout); // write the node to the output stream
                                  })
                     .parse(); // start the parsing process
  }
  doctotext::Info close_tag(doctotext::StandardTag::TAG_CLOSE_DOCUMENT);
  plain_text_writer.write_to(close_tag, std::cout);
  return 0;
}
/// [example_cpp]
