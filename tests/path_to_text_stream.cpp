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
#include <iostream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | out_stream;
    ensure(out_stream.str()) == "Data processing refers to the activities performed on raw data to convert it into meaningful information. It involves collecting, organizing, analyzing, and interpreting data to extract useful insights and support decision-making. This can include tasks such as sorting, filtering, summarizing, and transforming data through various computational and statistical methods. Data processing is essential in various fields, including business, science, and technology, as it enables organizations to derive valuable knowledge from large datasets, make informed decisions, and improve overall efficiency.\n\n";
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
