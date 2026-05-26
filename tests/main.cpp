/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#include "gtest/gtest.h"
#include "environment.h"
#include "log.h"
#include "log_json_stream_sink.h"
#include <iostream>

using namespace docwire;

int main(int argc, char* argv[])
{
    if (environment::get("DOCWIRE_TESTS_CONSOLE_LOGGING").value_or("0") == "1")
    {
        log::set_sink(log::json_stream_sink(std::clog));
        log::set_filter("*");
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
