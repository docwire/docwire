/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
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

    std::filesystem::path("document_processing_market_trends.odt") |
        content_type::detector{} | office_formats_parser{} |
        plain_text_exporter() |
        ai::local::llama::task("Classify to one of the following categories and "
                        "answer with exact category name: agreement, invoice, "
                        "report, legal, user manual, other:\n\n",
                        config) |
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
