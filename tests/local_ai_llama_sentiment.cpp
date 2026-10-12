#include "docwire.h"
#include "local_ai_llama_detect_sentiment.h"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  using namespace docwire;
  try {
    std::stringstream out_stream;
    docwire::ai::model_inference_config config;
    config.model_path = docwire::ai::llama::granite_model_path();
    config.max_tokens = docwire::ai::token_limit{256};
    config.n_ctx = docwire::ai::context_size{4096};
    config.n_threads = docwire::ai::thread_count{4};
    config.temp = docwire::ai::temperature{0.2f};
    config.min_probability = docwire::ai::min_p{0.05f};

    std::filesystem::path("data_processing_definition.doc") |
        content_type::detector{} | office_formats_parser{} |
        plain_text_exporter() | ai::local::llama::detect_sentiment(config) |
        out_stream;
    ensure(out_stream.str()) == "positive";
    std::cout << "Result: " + out_stream.str();

  } catch (const std::exception &e) {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
