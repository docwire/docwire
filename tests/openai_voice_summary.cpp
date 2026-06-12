#include "docwire.h"
#include <fstream>

int main(int argc, char* argv[])
{
  using namespace docwire;
  try
  {
    std::filesystem::path("1.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | openai::summarize(environment::get("OPENAI_API_KEY").value()) | openai::text_to_speech(environment::get("OPENAI_API_KEY").value()) | std::ofstream("summary.mp3");
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
