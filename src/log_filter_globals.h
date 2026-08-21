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

#ifndef DOCWIRE_LOG_FILTER_GLOBALS_H
#define DOCWIRE_LOG_FILTER_GLOBALS_H

#include "core_export.h"
#include "log_filter_rules.h"

#include <mutex>
#include <string>

namespace docwire::log::detail
{

DOCWIRE_CORE_EXPORT extern std::mutex g_log_filter_mutex;
DOCWIRE_CORE_EXPORT extern filter_spec g_log_filter;
DOCWIRE_CORE_EXPORT extern std::string g_log_filter_str;

} // namespace docwire::log::detail

#endif // DOCWIRE_LOG_FILTER_GLOBALS_H
