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

#include "boost_dll_wrapper.h"

#include <boost/dll/runtime_symbol_info.hpp>

namespace docwire::detail
{

std::filesystem::path this_line_location_helper(const this_line_location_t& this_line_location_instance)
{
    return boost::dll::symbol_location(this_line_location_instance).lexically_normal().string();
}

std::filesystem::path program_location()
{
    return std::filesystem::path(boost::dll::program_location().string());
}

} // namespace docwire::detail
