/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "docwire.h"

int main(int argc, char* argv[])
{
  using namespace df0;
  try
  {
    std::filesystem::path("test.zip") | content_type::detector{} | archives_parser{} | office_formats_parser{} | ocr_parser{} | plain_text_exporter() | std::cout;
  }
  catch (const std::exception& e)
  {
    std::cerr << errors::diagnostic_message(e) << std::endl;
    return 1;
  }
  return 0;
}
