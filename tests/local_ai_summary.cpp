#include "docwire.h"
#include "ensure.h"
#include "fuzzy_match.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | ai::local::summarize() | out_stream;
    ensure(fuzzy_match::ratio(out_stream.str(), "Data processing is the collection, organization, analysis, and interpretation of data.\n")) > 50;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
