#include "docwire/docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("../tests/data_processing_definition.doc") | content_type::detector{} | office_formats_parser() | PlainTextExporter() | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  std::cout << out_stream.str() << std::endl;

  return 0;
}
