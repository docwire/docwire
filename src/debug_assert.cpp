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

#include "debug_assert.h"

#ifndef NDEBUG
#include "diagnostic_message.h"
#include <iostream>
#include <cstdlib>

namespace docwire::errors
{
    [[noreturn]] void panic(std::exception_ptr eptr)
    {
        std::cerr << "Terminating due to contract violation: " << diagnostic_message(eptr) << std::endl;
        std::abort();
    }
}
#endif