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

#ifndef DOCWIRE_BOOST_DATETIME_WRAPPER_H
#define DOCWIRE_BOOST_DATETIME_WRAPPER_H

#include "core_export.h"

#include <string>

namespace docwire::detail
{

/**
 * @brief Returns the current local time as an ISO extended string with local timezone offset.
 *
 * @return A timestamp in the format:
 *         `YYYY-MM-DDTHH:MM:SS+HHMM` or `YYYY-MM-DDTHH:MM:SS-HHMM`.
 */
DOCWIRE_CORE_EXPORT std::string local_iso_datetime_with_timezone_offset();

} // namespace docwire::detail

#endif // DOCWIRE_BOOST_DATETIME_WRAPPER_H
