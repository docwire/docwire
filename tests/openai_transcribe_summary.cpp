#include "docwire.h"
#include <iostream>
#include <sstream>
int main(int argc, char* argv[])
{
  using namespace docwire;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.mp3") | openai::transcribe(environment::get("OPENAI_API_KEY").value()) | plain_text_exporter() | openai::summarize(environment::get("OPENAI_API_KEY").value(), openai::Model::gpt_4o) | out_stream;
    ensure(fuzzy_match::ratio(out_stream.str(), "Data processing involves converting raw data into meaningful information by collecting, organizing, analyzing, and interpreting it. This process includes tasks like sorting, filtering, summarizing, and transforming data using computational and statistical methods. It is crucial in fields like business, science, and technology, as it helps organizations extract valuable insights from large datasets, make informed decisions, and enhance efficiency.\n")) > 80;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
