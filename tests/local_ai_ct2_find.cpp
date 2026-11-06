/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "local_ai_ct2_find.h"
#include "df0.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | ai::local::ct2::find("data conversion") | out_stream;
    ensure(out_stream.str()).is_one_of({
      "Data processing refers to the activities performed on raw data to convert it into meaningful information.",
      "Data processing refers to the activities performed on raw data to convert it into meaningful information",
      "Data processing is the activities performed on raw data to convert it into meaningful information.",
      "Data processing is the activities performed on raw data to convert it into meaningful information"
    });
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
