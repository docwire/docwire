/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "contains_type.h" // IWYU pragma: keep
#include "error_tags.h"
#include "message_matchers.h" // IWYU pragma: keep
#include "ocr_parser.h"
#include "pipeline/input_element.hpp"
#include "pipeline/output_element.hpp"

using namespace df0;
using namespace testing;

TEST(ocr_parser, leptonica_stderr_capturer)
{
    try
    {
        data_source{std::string{"Incorrect image data"}, 
            mime_type{"image/jpeg"}, confidence::highest} |
            ocr_parser{} | std::vector<message_ptr>{};
        FAIL() << "ocr_parser should have thrown an exception";
    }
    catch (const std::exception& e)
    {
        ASSERT_TRUE(errors::contains_type<errors::uninterpretable_data>(e))
            << "Thrown exception diagnostic message:\n" << errors::diagnostic_message(e);
        ASSERT_THAT(errors::diagnostic_message(e), HasSubstr(
            "with context \"leptonica_stderr_capturer.contents(): Error in pixReadMem: Unknown format: no pix returned\""));
    }
}
