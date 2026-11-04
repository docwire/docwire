/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "docwire.h"
#include <iostream>
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("document_processing_market_trends.odt") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | openai::classify({ "agreement", "invoice", "report", "legal", "other"}, environment::get("OPENAI_API_KEY").value()) | out_stream;
    ensure(out_stream.str()) == "report\n";
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
