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

#ifndef DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H
#define DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H

#include "core_export.h"
#include <cstddef>

namespace df0::detail
{

/**
 * @brief Returns the next process-wide unique identifier value.
 *
 * @return A monotonically increasing size_t value.
 *
 * @note This function is implemented in docwire_core and exported from it.
 *       Keeping the counter here ensures identifiers remain unique across
 *       DLL boundaries.
 */
DOCWIRE_CORE_EXPORT size_t next_unique_identifier();

} // namespace df0::detail

#endif // DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H
