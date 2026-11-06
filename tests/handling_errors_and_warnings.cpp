/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#define DF0_ENABLE_SHORT_MACRO_NAMES

#include "df0.hpp"
#include "error_tags.h"
#include "error.h"
#include "make_error.h"
#include "contains_type.h"
#include <filesystem>
#include <iostream>
#include <sstream>

void retry()
{
  // Simulated retry logic for demonstration purposes.
}

void demonstrate_error_handling()
{
  using namespace df0;

  // Snippet 1: categorized and tagged errors
  try
  {
    throw make_error(errors::network_failure{});
  }
  catch (const df0::errors::base& e)
  {
    if (df0::errors::contains_type<df0::errors::network_failure>(e))
      retry();
  }

  // Snippet 2: embedded source location
  try
  {
    throw make_error("Demonstration error");
  }
  catch (const df0::errors::base& e)
  {
    std::cerr << e.location.file_name() << ":" << e.location.line() << std::endl;
  }

  // Snippet 3: secure context retrieval
  try
  {
    throw make_error(std::filesystem::path{"/secret/file.txt"});
  }
  catch (df0::errors::base& e)
  {
    std::cerr << df0::errors::diagnostic_message(e) << std::endl;
    // or
    if (auto* impl_ptr = dynamic_cast<df0::errors::impl<std::filesystem::path>*>(&e))
    {
      auto fn = impl_ptr->context;
      (void)fn; // avoid unused variable warning
    }
  }
}

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") |
      content_type::by_file_extension::detector{} |
      office_formats_parser{} |
      plain_text_exporter() |
      [](message_ptr msg, const message_callbacks& emit_message)
	    {
	      if (msg->is<std::exception_ptr>())
        {
		      std::clog << "[WARNING] " <<
            errors::diagnostic_message(msg->get<std::exception_ptr>()) <<
            std::endl;
        }
        return emit_message(std::move(msg));
	    } |
      out_stream;
  }
  catch (const std::exception& e)
  {
    std::cerr << "[ERROR] " << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  demonstrate_error_handling();

  return 0;
}
