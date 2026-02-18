#include "docwire/docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
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
        local_ai::model_chain_element("Write a short summary for this text:\n\n") |
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
