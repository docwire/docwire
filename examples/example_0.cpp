/// [example_cpp]
#include <iostream>

#include "input.h"
#include "importer.h"
#include "plain_text_exporter.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    using namespace docwire;
    Input(argv[1]) |
      Importer() |
      PlainTextExporter(std::cout); // parse file and print to standard output
  }
  return 0;
}
/// [example_cpp]
