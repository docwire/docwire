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

#ifndef DOCWIRE_LOG_FILE_EXTENSION_H
#define DOCWIRE_LOG_FILE_EXTENSION_H

#include "file_extension.h"
#include "log.h"

namespace docwire
{

/**
* @brief Logs the file extension to a record stream.
*
* @param log_stream The record stream to log to.
*/
inline log_record_stream& operator<<(log_record_stream& log_stream, const file_extension& ext)
{
	log_stream << docwire_log_streamable_obj(ext, ext.string());
	return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_FILE_EXTENSION_H
