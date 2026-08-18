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

#ifndef DOCWIRE_ENVIRONMENT_H
#define DOCWIRE_ENVIRONMENT_H

#include "core_export.h"
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

namespace docwire::environment
{
    inline std::optional<std::string> get(std::string_view name)
    {
        const std::string name_str{name};
        const char* value = std::getenv(name_str.c_str());
        if (value)
        {
            return std::string{value};
        }
        return std::nullopt;
    }
}

#endif // DOCWIRE_ENVIRONMENT_H
