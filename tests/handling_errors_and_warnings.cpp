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
      office_formats_parser{} |
      PlainTextExporter() |
      [](Tag&& tag, const emission_callbacks& emit_tag)
	    {
	      if (std::holds_alternative<std::exception_ptr>(tag))
        {
		      std::clog << "[WARNING] " <<
            errors::diagnostic_message(std::get<std::exception_ptr>(tag)) <<
            std::endl;
        }
        return emit_tag(std::move(tag));
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
