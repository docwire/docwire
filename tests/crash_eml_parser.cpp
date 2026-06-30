#include "docwire.h"
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
int main(int argc, char *argv[]) {
  using namespace docwire;

  if (argc < 2) {
    std::cerr << "No crash eml files supplied\n";
    return 1;
  }

  for (int i = 1; i < argc; ++i) {
    fs::path file = argv[i];
    std::stringstream out_stream;
    std::cout << "Testing " << file << "\n";
    try {
      file | content_type::detector{} | mail_parser{} | plain_text_exporter() |
          out_stream;
    } catch (const std::exception &e) {
      std::cerr << "Exception while parsing " << file << '\n'
                << errors::diagnostic_message(e) << std::endl;
      return 1;
    }
  }

  return 0;
}