/// [example_cpp]
#include <iostream>
#include <fstream>
#include <memory>

#include "importer.h"
#include "exporter.h"
#include "transformer.h"
#include "parsing_chain.h"

/**
 * @example example_1.cpp
 * @brief This example shows how to use the API to parse a file and export it to plain text and html.
 */
int main(int argc, char* argv[])
{
  // parse file and print to output.txt file
  std::ifstream(argv[1], std::ios_base::in|std::ios_base::binary)
    | doctotext::Importer()
    | doctotext::PlainTextExporter()
    | std::ofstream("output.txt");

  // parse file and print to output.html file
  std::ifstream(argv[1], std::ios_base::in|std::ios_base::binary)
    | doctotext::Importer()
    | doctotext::HtmlExporter()
    | std::ofstream("output.html");

  return 0;
}
/// [example_cpp]
