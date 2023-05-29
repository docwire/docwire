/// [example_cpp]
#include <iostream>
#include <memory>

#include "importer.h"
#include "exporter.h"
#include "parsing_chain.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    doctotext::Importer(argv[1]) | doctotext::PlainTextExporter()
                                 | std::cout; // parse file and print to standard output
  }
  return 0;
}
/// [example_cpp]
