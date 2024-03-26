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

#ifndef DOCWIRE_LOG_CTIME_H
#define DOCWIRE_LOG_CTIME_H

#include <ctime>
#include "log.h"
#include <sstream>

namespace docwire
{

inline log_record_stream& operator<<(log_record_stream& log_stream, const tm& time)
{
    std::ostringstream date_stream;
    date_stream << std::put_time(&time, "%Y-%m-%d %H:%M:%S");
    log_stream << date_stream.str();
    return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_CTIME_H
