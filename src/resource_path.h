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

#ifndef DOCWIRE_RESOURCE_PATH_H
#define DOCWIRE_RESOURCE_PATH_H

#include "boost_dll_wrapper.h"
#include "core_export.h"
#include "error_tags.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>
#include "make_error.h"

namespace docwire
{


inline std::filesystem::path this_line_location()
{
    detail::this_line_location_t& f = this_line_location;
    return detail::this_line_location_helper(f);
}

/**
 * @brief Locates a resource file or directory at runtime.
 *
 * This function searches for the specified resource in standard locations relative to the
 * running executable and the shared library. It supports both build-tree layouts (via .path files)
 * and installed layouts (relative to bin/ or lib/).
 *
 * @param resource_rel_path The relative path of the resource to find (e.g., "docwire/data.bin").
 * @return The absolute path to the found resource.
 * @throws docwire::errors::base if the resource cannot be found.
 */
inline std::filesystem::path resource_path(const std::filesystem::path& resource_rel_path)
{
    std::vector<std::filesystem::path> paths_to_check;

    auto add_search_paths = [&](const std::filesystem::path& base_path) {
        if (base_path.empty())
            return;

        auto base_dir = base_path.parent_path();

        // Check ./share
        paths_to_check.push_back(base_dir / "share" / resource_rel_path);

        // Check ../share
        paths_to_check.push_back(base_dir.parent_path() / "share" / resource_rel_path);

        // Handle vcpkg debug layout: .../debug/bin/ -> .../share/
        if (base_dir.parent_path().filename() == "debug")
        {
            paths_to_check.push_back(base_dir.parent_path().parent_path() / "share" / resource_rel_path);
        }
    };

    add_search_paths(this_line_location());
    add_search_paths(detail::program_location());

    // Remove duplicates while preserving order
    std::vector<std::filesystem::path> unique_paths;
    for (const auto& path : paths_to_check)
    {
        if (std::find(unique_paths.begin(), unique_paths.end(), path) == unique_paths.end())
        {
            unique_paths.push_back(path);
        }
    }

    for (const auto& path : unique_paths)
    {
        if (std::filesystem::exists(path))
            return path;

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

    throw DOCWIRE_MAKE_ERROR("Resource not found", resource_rel_path, errors::program_corrupted{});
}

} // namespace docwire

#endif //DOCWIRE_RESOURCE_PATH_H
