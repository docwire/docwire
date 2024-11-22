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

#ifndef DOCWIRE_LOG_DATA_SOURCE_H
#define DOCWIRE_LOG_DATA_SOURCE_H

#include "data_source.h"
#include "log.h"
#include "log_file_extension.h"

namespace docwire
{

inline log_record_stream& operator<<(log_record_stream& log_stream, const data_source& data)
{
	log_stream << docwire_log_streamable_obj(data, data.path(), data.file_extension());
	return log_stream;
}

} // namespace docwire

#endif // DOCWIRE_LOG_DATA_SOURCE_H
