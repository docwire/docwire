#include "docwire.h"
#include <cassert>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") |
      content_type::by_file_extension::detector{} |
      ParseDetectedFormat<OfficeFormatsParserProvider>() |
      PlainTextExporter() |
      [](Info& info)
	    {
	      if (std::holds_alternative<std::exception_ptr>(info.tag))
		      std::clog << "[WARNING] " <<
            errors::diagnostic_message(std::get<std::exception_ptr>(info.tag)) <<
            std::endl;
	    } |
      out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << "[ERROR] " << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
