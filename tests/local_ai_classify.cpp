#include "docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("document_processing_market_trends.odt") | content_type::detector{} | office_formats_parser{} | PlainTextExporter() | ai::local::task("Classify to one of the following categories and answer with exact category name: agreement, invoice, report, legal, user manual, other:\n\n") | out_stream;
    ensure(out_stream.str()) == "report";
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
