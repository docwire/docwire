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

// Legacy global DOCWIRE_TESTS_CONSOLE_LOGGING support was removed with
// docwire::log. The injected-auditor architecture intentionally has no
// process-global sink or filter, so this test main must not re-introduce
// global logging state.
//
// TODO(tests): Add a stateful-auditor test variant to the parser/export
// suites. The current practical stateful type is audit::json_stream_auditor,
// because exported component specializations already exist for it.
// A lightweight recording_auditor will become preferable once those
// templates are moved to public header-only definitions.
//
// When the test matrix is available, control the second variant with an
// explicit test helper rather than a global environment variable; the
// helper may read DOCWIRE_TESTS_CONSOLE_LOGGING to choose
// json_stream_auditor over null_auditor/recording_auditor.

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
