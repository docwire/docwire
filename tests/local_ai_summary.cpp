#include "docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | ai::local::summarize() | out_stream;
    ensure(out_stream.str()).is_one_of({
        "Data processing is the collection, organization, analysis, and interpretation of data to extract useful insights and support decision-making.",
        "Data processing is the process of transforming raw data into meaningful information.",
        "Data processing is the collection, organization, analysis, and interpretation of data."
    });
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
