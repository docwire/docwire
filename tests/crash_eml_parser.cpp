#include "docwire.h"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("crash_test_endboundary_first.eml") | content_type::detector{} 
    | mail_parser{} | plain_text_exporter() | out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
