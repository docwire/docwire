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

#ifndef DF0_BOOST_DLL_WRAPPER_H
#define DF0_BOOST_DLL_WRAPPER_H

#include "core_export.h"

#include <filesystem>

namespace df0::detail
{

using this_line_location_t = std::filesystem::path();

DF0_CORE_EXPORT std::filesystem::path this_line_location_helper(
    const this_line_location_t& this_line_location_instance);

DF0_CORE_EXPORT std::filesystem::path program_location();

} // namespace df0::detail

#endif // DF0_BOOST_DLL_WRAPPER_H
