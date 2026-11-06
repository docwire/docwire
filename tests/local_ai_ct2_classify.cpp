/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("document_processing_market_trends.odt") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | ai::local::ct2::task("Classify to one of the following categories and answer with exact category name: agreement, invoice, report, legal, user manual, other:\n\n") | out_stream;
    ensure(out_stream.str()) == "report";
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
