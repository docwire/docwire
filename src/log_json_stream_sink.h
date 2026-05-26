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

#ifndef DOCWIRE_LOG_JSON_STREAM_SINK_H
#define DOCWIRE_LOG_JSON_STREAM_SINK_H

#include "core_export.h"
#include <functional>
#include "log_core.h"
#include <ostream>
#include "ref_or_owned.h"

namespace docwire::log
{
DOCWIRE_CORE_EXPORT std::function<void(const record&)> json_stream_sink(ref_or_owned<std::ostream> stream);
}

#endif // DOCWIRE_LOG_JSON_STREAM_SINK_H
