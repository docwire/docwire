/// [example_cpp]
#include <iostream>
#include <memory>

#include "input.h"
#include "parser.h"
#include "importer.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "transformer_func.h"
#include "parsing_chain.h"

/**
 * @example example_3.cpp
 * @brief This example shows how to use transformer object to filter out mails by keyword "Hello" in subject
 */
int main(int argc, char* argv[])
{
  using namespace docwire;
  Input(argv[1]) |
  Importer()
    | TransformerFunc([](Info &info) // Create an importer from file name and connect it to transformer
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
      })
    | PlainTextExporter() // sets exporter to plain text
    | Output(std::cout);
  return 0;
}
/// [example_cpp]
