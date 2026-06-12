#include "docwire.h"
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::ifstream("data_processing_definition.docx", std::ios_base::binary) | content_type::detector{} | office_formats_parser{} | html_exporter() | out_stream;
    ensure(out_stream.str()).contains("Data processing refers to the activities");
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
