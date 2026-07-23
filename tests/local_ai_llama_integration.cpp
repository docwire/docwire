#include "docwire.h"
#include "resource_path.h"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  using namespace docwire;

  std::filesystem::path model_path;
  try {
  	model_path = resource_path("granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf");
  } catch (const std::exception &) {
    std::cerr << "Model not found via resource_path(); skipping test.\n";
    return 0;
  }

  try {
    std::stringstream out_stream;
    docwire::ai::model_inference_config config;
    config.model_path = model_path.string();
    config.max_tokens = docwire::ai::token_limit{256};
    config.n_ctx = docwire::ai::context_size{4096};
    config.n_threads = docwire::ai::thread_count{4};
    config.temp = docwire::ai::temperature{0.2f};
    config.min_probability = docwire::ai::min_p{0.05f};
    auto runner = std::make_shared<docwire::ai::llama::llama_runner>(config);

    std::ofstream ofs("output.txt");
    if (!ofs)
    {
    	throw std::runtime_error("Failed to open output.txt for writing");
    }
    std::filesystem::path("data_processing_definition.doc") |
    	content_type::detector{} | office_formats_parser{} | plain_text_exporter() |
        ai::local::task("Summarize:\n\n", runner) | out_stream;

    if (out_stream.str().empty())
    {
        throw std::runtime_error("Generated summary is empty");
    }
    ofs << out_stream.str();
    ofs.close();
    std::cout << "Text exported to output.txt" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
