/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
  using namespace df0;
  try {
    std::stringstream out_stream;
    df0::ai::model_inference_config config;
    config.model_path = df0::ai::llama::granite_model_path();
    config.max_tokens = df0::ai::token_limit{256};
    config.n_ctx = df0::ai::context_size{4096};
    config.n_threads = df0::ai::thread_count{4};
    config.temp = df0::ai::temperature{0.2f};
    config.min_probability = df0::ai::min_p{0.05f};

    std::ofstream ofs("output.txt");
    if (!ofs)
    {
    	throw std::runtime_error("Failed to open output.txt for writing");
    }
    std::filesystem::path("data_processing_definition.doc") |
    	content_type::detector{} | office_formats_parser{} | plain_text_exporter() |
        ai::local::llama::task("Summarize:\n\n", config) | out_stream;

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
