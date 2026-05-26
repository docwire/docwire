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

#ifndef DOCWIRE_WITH_SOURCE_LOCATION_H
#define DOCWIRE_WITH_SOURCE_LOCATION_H

#include "source_location.h"

namespace docwire::detail
{
    /**
     * @brief Helper struct to capture the source location of a call site.
     * This is implicitly constructed from a value of type T.
     * @tparam T The type of the value being wrapped.
     */
    template <typename T>
    struct with_source_location
    {
        T value;
        source_location location;

        constexpr with_source_location(T val, const source_location& loc = source_location::current())
            : value(val), location(loc) {}
    };
}

#endif // DOCWIRE_WITH_SOURCE_LOCATION_H