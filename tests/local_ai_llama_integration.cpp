#include "docwire.h"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
    using namespace docwire;
    std::stringstream out_stream;
    docwire::ai::model_inference_config config;
    config.model_path = "../models/qwen2-7b-instruct-q4_k_m.gguf";
    config.max_tokens = docwire::ai::token_limit{256};
    config.n_ctx = docwire::ai::context_size{4096};
    config.n_threads = docwire::ai::thread_count{4};
    config.temp = docwire::ai::temperature{0.2f};
    config.min_probability = docwire::ai::min_p{0.05f};
    auto runner = std::make_shared<docwire::ai::llama::llama_runner>(config);

    try {
    	std::ofstream ofs("output.txt");
        data_source(std::string("LLMs help process long documents."), mime_type{"text/plain"},
                    confidence::highest) |
            ai::local::task("Summarize:\n\n", runner) | out_stream | ofs;
        std::cout << "Text exported to output.txt" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << errors::diagnostic_message(e) << std::endl;
        return 1;
    }

    return 0;
}
