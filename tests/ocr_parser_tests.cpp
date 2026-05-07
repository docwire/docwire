/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "contains_type.h" // IWYU pragma: keep
#include "error_tags.h"
#include "message_matchers.h" // IWYU pragma: keep
#include "ocr_parser.h"
#include "input.h"
#include "output.h"

using namespace docwire;
using namespace testing;

TEST(OCRParser, leptonica_stderr_capturer)
{
    try
    {
        data_source{std::string{"Incorrect image data"}, 
            mime_type{"image/jpeg"}, confidence::highest} |
            OCRParser{} | std::vector<message_ptr>{};
        FAIL() << "OCRParser should have thrown an exception";
    }
    catch (const std::exception& e)
    {
        ASSERT_TRUE(errors::contains_type<errors::uninterpretable_data>(e))
            << "Thrown exception diagnostic message:\n" << errors::diagnostic_message(e);
        ASSERT_THAT(errors::diagnostic_message(e), HasSubstr(
            "with context \"leptonica_stderr_capturer.contents(): Error in pixReadMem: Unknown format: no pix returned\""));
    }
}
