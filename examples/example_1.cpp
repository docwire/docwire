/// [example_cpp]
#include <iostream>
#include <fstream>
#include <memory>

#include "input.h"
#include "importer.h"
#include "exporter.h"
#include "transformer_func.h"
#include "parsing_chain.h"

/**
 * @example example_1.cpp
 * @brief This example shows how to use the API to parse a file and export it to plain text and html.
 */
int main(int argc, char* argv[])
{
  using namespace docwire;
  // parse file and print to output.txt file
  Input(std::ifstream(argv[1], std::ios_base::in|std::ios_base::binary))
    | Importer()
    | PlainTextExporter(std::ofstream("output.txt"));

  // parse file and print to output.html file
  Input(std::ifstream(argv[1], std::ios_base::in|std::ios_base::binary))
    | Importer()
    | HtmlExporter()
    | PlainTextExporter(std::ofstream("output.html"));

  return 0;
}
/// [example_cpp]
