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

#ifndef DOCWIRE_RESOURCE_PATH_H
#define DOCWIRE_RESOURCE_PATH_H

#include "core_export.h"
#include <filesystem>

namespace docwire
{

namespace detail
{
typedef std::filesystem::path(this_line_location_t)();
DOCWIRE_CORE_EXPORT std::filesystem::path this_line_location_helper(const this_line_location_t& this_line_location_instance);
DOCWIRE_CORE_EXPORT std::filesystem::path resource_path(const std::filesystem::path& module_path, const std::filesystem::path& resource_rel_path);
}

// Anonymous namespace to make sure that inline methods are instantiated in
// current translation unit and are not shadowed by instantiations from other units.
namespace
{

static inline std::filesystem::path this_line_location()
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
static inline std::filesystem::path resource_path(const std::filesystem::path& resource_rel_path)
{
    return docwire::detail::resource_path(this_line_location(), resource_rel_path);
}

} // anonymous namespace

} // namespace docwire

#endif //DOCWIRE_RESOURCE_PATH_H
