/// [example_cpp]
#include <iostream>

#include "input.h"
#include "importer.h"
#include "exporter.h"

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    doctotext::Input(argv[1]) |
      doctotext::Importer() |
      doctotext::PlainTextExporter() |
      std::cout; // parse file and print to standard output
  }
  return 0;
}
/// [example_cpp]
