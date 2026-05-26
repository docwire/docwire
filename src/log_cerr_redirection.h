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

#ifndef DOCWIRE_LOG_CERR_REDIRECTION_H
#define DOCWIRE_LOG_CERR_REDIRECTION_H

#include "core_export.h"
#include "pimpl.h"
#include "source_location.h"
#include <string_view>

namespace docwire::log
{

/// @brief Tag for log entries that contain content redirected from `stderr`.
struct DOCWIRE_CORE_EXPORT stderr_redirect { static constexpr std::string_view string() { return "stderr_redirect"; } };

class DOCWIRE_CORE_EXPORT cerr_redirection : public with_pimpl<cerr_redirection>
{
public:
	/**
	 * @brief Constructs a cerr_redirection object, capturing the source location.
	 * @param location The source location where the redirection is initiated.
	 */
	explicit cerr_redirection(const source_location& location = source_location::current());

	cerr_redirection(const cerr_redirection&) = delete;
	cerr_redirection& operator=(const cerr_redirection&) = delete;
	~cerr_redirection();
	void redirect();
	void restore();
};

} // namespace docwire::log

#endif // DOCWIRE_LOG_CERR_REDIRECTION_H
