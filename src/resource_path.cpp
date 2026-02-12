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
#include <vector>
#include <fstream>
#include <algorithm>
#include "make_error.h"
#include "error_tags.h"

namespace docwire
{

namespace detail
{

std::filesystem::path this_line_location_helper(const this_line_location_t& this_line_location_instance)
{
    return boost::dll::symbol_location(this_line_location_instance).lexically_normal().string();
}

std::filesystem::path resource_path(const std::filesystem::path& module_path, const std::filesystem::path& resource_rel_path)
{
    std::vector<std::filesystem::path> paths_to_check;

    auto add_search_paths = [&](const std::filesystem::path& base_path) {
        if (base_path.empty()) return;
        auto base_dir = base_path.parent_path();
        
        // 1. Check ./share (Safe for build trees and Windows apps)
        paths_to_check.push_back(base_dir / "share" / resource_rel_path);
        // 2. Check ../share (Standard for Linux installations)
        paths_to_check.push_back(base_dir.parent_path() / "share" / resource_rel_path);

        // 3. Handle vcpkg debug layout: .../debug/bin/ -> .../share/
        if (base_dir.parent_path().filename() == "debug") {
            paths_to_check.push_back(base_dir.parent_path().parent_path() / "share" / resource_rel_path);
        }
    };

    // 1. Check paths relative to the executable (program).
    // Essential for build tree tests (app in tests/, lib in src/) and Windows apps.
    add_search_paths(std::filesystem::path(boost::dll::program_location().string()));

    // 2. Check paths relative to the library (module).
    // Essential for system installations and plugins where the app is generic (e.g. python).
    add_search_paths(module_path);

    // Filter duplicates while preserving order
    std::vector<std::filesystem::path> unique_paths;
    for (const auto& path : paths_to_check) {
        if (std::find(unique_paths.begin(), unique_paths.end(), path) == unique_paths.end()) {
            unique_paths.push_back(path);
        }
    }

    for (const auto& path : unique_paths)
    {
        // 1. Check if the resource exists directly at the candidate path.
        //    This handles standard installations.
        if (std::filesystem::exists(path))
            return path;

        // 2. Check for a .path file, which is a text file containing the absolute path
        //    to the resource. This is used in the build tree to avoid copying large
        //    resource files into the build output directory.
        auto path_file = path;
        path_file += ".path";
        if (std::filesystem::exists(path_file))
        {
            std::ifstream ifs(path_file);
            std::string redirected_path;
            if (std::getline(ifs, redirected_path) && !redirected_path.empty() && std::filesystem::exists(redirected_path))
                return redirected_path;
        }
    }

    throw make_error("Resource not found", resource_rel_path, errors::program_corrupted{});
}

} // namespace detail

} // namespace docwire
