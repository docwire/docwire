#include "docwire/docwire.h"
#include <sstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  // This block is used by the after-installation tests to verify that the
  // example executable is built with the correct configuration (Debug/Release).
  if (argc > 1 && std::string(argv[1]) == "--check-build-type")
  {
#ifdef NDEBUG
    std::cout << "Build type: Release" << std::endl;
#else
    std::cout << "Build type: Debug" << std::endl;
#endif
    return 0;
  }

  using namespace docwire;
  std::stringstream out_stream;

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <path_to_doc_file>" << std::endl;
    return 1;
  }

  try
  {
    std::filesystem::path(argv[1]) |
        content_type::detector{} |
        office_formats_parser() |
        PlainTextExporter() |
        out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  std::cout << out_stream.str() << std::endl;

  return 0;
}
