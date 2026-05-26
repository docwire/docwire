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

#ifndef DOCWIRE_JSON_SERIALIZATION_H
#define DOCWIRE_JSON_SERIALIZATION_H

#include "core_export.h"
#include "serialization_base.h"

namespace docwire::serialization
{

/**
 * @brief Converts a `docwire::serialization::value` to a JSON string.
 *
 * This allows the generic serialization mechanism to be used for JSON-specific
 * tasks like stringification or structured logging.
 *
 * @param s_val The `docwire::serialization::value` to convert.
 * @return A string containing the JSON representation of the value.
 */
DOCWIRE_CORE_EXPORT std::string to_json(const value& s_val);

} // namespace docwire::serialization

#endif // DOCWIRE_JSON_SERIALIZATION_H
