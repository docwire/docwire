#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("test.zip") | content_type::detector{} | archives_parser{} | office_formats_parser{} | ocr_parser{} | plain_text_exporter() | std::cout;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  return 0;
}
