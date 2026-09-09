#include "docwire.h"
#include "resource_path.h"
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  using namespace docwire;
  std::filesystem::path model_path;
  try {
    model_path = resource_path("granite-4-1b-q8-0/granite-4.0-1b-Q8_0.gguf");
  } catch (const std::exception &) {
    std::cerr << "Model not found via resource_path(); skipping test.\n";
    return 1;
  }

  try {
    std::stringstream out_stream;
    docwire::ai::model_inference_config config;
    config.model_path = model_path;
    config.max_tokens = docwire::ai::token_limit{256};
    config.n_ctx = docwire::ai::context_size{4096};
    config.n_threads = docwire::ai::thread_count{4};
    config.temp = docwire::ai::temperature{0.2f};
    config.min_probability = docwire::ai::min_p{0.05f};
    auto runner = std::make_shared<docwire::ai::llama::llama_runner>(config);

    std::filesystem::path("data_processing_definition.doc") |
        content_type::detector{} | office_formats_parser{} |
        plain_text_exporter() |
        ai::local::task("Find exact sentence present about \"data conversion\" in the "
                        "following text:\n\n",
                        runner) |
        out_stream;
    ensure(out_stream.str())
        .is_one_of({"Data processing refers to the activities performed on raw "
                    "data to convert it into meaningful information.",
                    "Data processing is the activities performed on raw data "
                    "to convert it into meaningful information."});
    std::cout << "Result: " + out_stream.str();

  } catch (const std::exception &e) {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
