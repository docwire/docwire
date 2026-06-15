#include "docwire.h"
#include <filesystem>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | ai::local::task("Find sentence about \"data conversion\" in the following text:\n\n") | out_stream;
    ensure(out_stream.str()).is_one_of({
      "Data processing refers to the activities performed on raw data to convert it into meaningful information.",
      "Data processing is the activities performed on raw data to convert it into meaningful information."
    });
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
