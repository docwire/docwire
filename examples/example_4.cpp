/// [example_cpp]
#include <iostream>
#include <memory>

#include "input.h"
#include "parser.h"
#include "importer.h"
#include "exporter.h"
#include "transformer_func.h"
#include "parsing_chain.h"

/**
 * @example example_4.cpp
 * @brief This example shows how to connect together many transformers. In this example we have two transformer. The first filter
 * out mails by keyword "Hello". The second one cancels all process if reach the limit of 10 mails.
 */
int main(int argc, char* argv[])
{
  using namespace docwire;
  Input(argv[1]) |
  Importer() |
    TransformerFunc([](Info &info) // Create an input from file name, importer and connect them to transformer
    {
      if (info.tag_name == StandardTag::TAG_MAIL) // if current node is mail
      {
        auto subject = info.getAttributeValue<std::string>("subject"); // get the subject attribute
        if (subject) // if subject attribute exists
        {
          if (subject->find("Hello") != std::string::npos) // if subject contains "Hello"
          {
            info.skip = true; // skip the current node
          }
        }
      }
    }) |
    TransformerFunc([counter = 0, max_mails = 1](Info &info) mutable // Create a transformer and connect it to previous transformer
    {
      if (info.tag_name == StandardTag::TAG_MAIL) // if current node is mail
      {
        if (++counter > max_mails) // if counter is greater than max_mails
        {
          info.cancel = true; // cancel the parsing process
        }
      }
    }) |
    PlainTextExporter(std::cout); // sets exporter to plain text
 return 0;
}
/// [example_cpp]
