#include "docwire.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace docwire;

  try
  {
    std::filesystem::path("1.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | openai::detect_sentiment(environment::get("OPENAI_API_KEY").value()) | std::cout;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
