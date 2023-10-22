/// [example_cpp]
#include <iostream>
#include <fstream>
#include <memory>

#include "input.h"
#include "importer.h"
#include "output.h"
#include "plain_text_exporter.h"

/**
 * @example example_2.cpp
 * @brief This example shows how to use the API to parse multiple files.
 */
int main(int argc, char* argv[])
{
  using namespace docwire;
  auto chain = Importer()
             | PlainTextExporter()
             | Output(std::cout);  // create a chain of steps to parse a file
  for (int i = 1; i < argc; ++i)
  {
    std::cout << "Parsing file " << argv[i] << std::endl;
    Input(std::ifstream(argv[i], std::ios_base::in|std::ios_base::binary)) | chain; // set the input file as an input stream
    std::cout << std::endl;
  }

  return 0;
}
/// [example_cpp]
