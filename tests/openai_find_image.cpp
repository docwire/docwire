/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "df0.hpp"
#include "environment.h"
#include <sstream>

int main(int argc, char* argv[])
{
  using namespace df0;
  std::stringstream out_stream;

  try
  {
    std::filesystem::path("scene_1.png") | content_type::detector{} | openai::find("tree", environment::get("OPENAI_API_KEY").value(), openai::model::gpt_4o) | out_stream;
    ensure(fuzzy_match::ratio(out_stream.str(), "2\n- A tree is located on the left side of the image near the people.\n- Another tree is in the background near the center of the image.\n")) > 80;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }

  return 0;
}
