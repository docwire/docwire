/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include <fstream>

int main(int argc, char* argv[])
{
  using namespace df0;

  try
  {
    auto chain = content_type::detector{} | office_formats_parser{} | plain_text_exporter() | std::cout; // create a chain of steps to parse a file
    for (int i = 1; i < 3; ++i)
      std::ifstream(std::to_string(i) + ".docx", std::ios_base::binary) | chain; // set the input file as an input stream
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
