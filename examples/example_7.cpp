/// [example_cpp]
#include <iostream>
#include "simple_extractor.h"

/**
 * @example example_7.cpp
 */
int main(int argc, char* argv[])
{
  doctotext::SimpleExtractor simple_extractor(argv[1]); // create a simple extractor
  std::cout << simple_extractor.getPlainText(); // print the plain text of the document
}
/// [example_cpp]