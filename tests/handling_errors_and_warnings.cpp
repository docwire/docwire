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
      [](message_ptr msg, const message_callbacks& emit_message)
	    {
	      if (msg->is<std::exception_ptr>())
        {
		      std::clog << "[WARNING] " <<
            errors::diagnostic_message(msg->get<std::exception_ptr>()) <<
            std::endl;
        }
        return emit_message(std::move(msg));
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
