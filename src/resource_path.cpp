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

#include "resource_path.h"

#include <boost/dll/runtime_symbol_info.hpp>
#include <fstream>

namespace docwire
{

std::filesystem::path this_line_location_helper(const this_line_location_t& this_line_location_instance)
{
    return boost::dll::symbol_location(this_line_location_instance).lexically_normal().string();
}

std::filesystem::path resource_path(const std::filesystem::path& module_path, const std::filesystem::path& resource_rel_path)
{
    auto path = module_path.parent_path();
    if (path.parent_path().filename() == "debug") {
        // If we are in a vcpkg debug build, adjust the path to access resources in the release directory
        path = path.parent_path().parent_path() / "share" / resource_rel_path;
    } else {
        // For release builds, the resources are directly in the ../share directory
        path = path.parent_path() / "share" / resource_rel_path;
    }
    if (std::filesystem::exists(path.string() + ".path"))
    {
        // Read path from path file
        std::ifstream ifs(path.string() + ".path");
        std::string redirected_path;
        std::getline(ifs, redirected_path);
        path = redirected_path;
    }
    return path;
}

} // namespace docwire
