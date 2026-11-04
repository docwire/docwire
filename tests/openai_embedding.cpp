/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"

int main(int argc, char* argv[])
{
  using namespace df0;
  std::vector<message_ptr> out_msgs;

  try
  {
    std::filesystem::path("data_processing_definition.doc") | content_type::detector{} | office_formats_parser{} | plain_text_exporter() | openai::embed(environment::get("OPENAI_API_KEY").value()) | out_msgs;
    ensure(out_msgs.size()) == 1;
    ensure(out_msgs[0]->is<ai::embedding>()) == true;
    ensure(out_msgs[0]->get<ai::embedding>().values.size()) == 1536;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
