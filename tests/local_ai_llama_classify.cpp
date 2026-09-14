#include "docwire.h"
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
    auto runner = std::make_shared<docwire::ai::llama::llama_runner>(config);

    std::filesystem::path("document_processing_market_trends.odt") |
        content_type::detector{} | office_formats_parser{} |
        plain_text_exporter() |
        ai::local::task("Classify to one of the following categories and "
                        "answer with exact category name: agreement, invoice, "
                        "report, legal, user manual, other:\n\n",
                        runner) |
        out_stream;

    if (out_stream.str().empty()) {
      throw std::runtime_error("Generated output is empty");
    }
    std::cout << "Result: " + out_stream.str();
    ensure(out_stream.str()) == "report";

  } catch (const std::exception &e) {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
