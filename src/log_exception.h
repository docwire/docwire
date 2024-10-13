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

#ifndef DOCWIRE_LOG_EXCEPTION_H
#define DOCWIRE_LOG_EXCEPTION_H

#include "log.h"
#include "exception_utils.h"

namespace docwire
{

inline log_record_stream& operator<<(log_record_stream& log_stream, const std::exception_ptr eptr)
{
	if (eptr)
		log_stream << begin_complex() <<
            docwire_log_streamable_type_of(eptr) <<
            std::make_pair("diagnostic_message", errors::diagnostic_message(eptr)) <<
            end_complex();
	else
		log_stream << nullptr;
	return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_EXCEPTION_H
