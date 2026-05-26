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

#ifndef DOCWIRE_DIAGNOSTIC_MESSAGE_H
#define DOCWIRE_DIAGNOSTIC_MESSAGE_H

#include "core_export.h"
#include <string>
#include <exception>

namespace docwire::errors
{

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
 */
DOCWIRE_CORE_EXPORT std::string diagnostic_message(const std::exception& e);

/**
 * @brief Generates a diagnostic message for the given nested exceptions chain.
 */
DOCWIRE_CORE_EXPORT std::string diagnostic_message(std::exception_ptr eptr);

} // namespace docwire::errors

#endif // DOCWIRE_DIAGNOSTIC_MESSAGE_H
