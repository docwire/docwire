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
#include "log_cerr_redirection_globals.h"
#include "log_core.h"
#include "log_entry.h"
#include "serialization_base.h"
#include "source_location.h"

#include <iostream>
#include <mutex>
#include <sstream>
#include <streambuf>
#include <string_view>

namespace docwire::log
{

/// @brief Tag for log entries that contain content redirected from `stderr`.
struct stderr_redirect { static constexpr std::string_view string() { return "stderr_redirect"; } };

class cerr_redirection
{
public:
	/**
	 * @brief Constructs a cerr_redirection object, capturing the source location.
	 * @param location The source location where the redirection is initiated.
	 */
	explicit cerr_redirection(const source_location& location = source_location::current())
	{
#ifndef NDEBUG
		m_location = location;
#endif
		redirect();
	}

	cerr_redirection(const cerr_redirection&) = delete;
	cerr_redirection& operator=(const cerr_redirection&) = delete;

	~cerr_redirection()
	{
		if (m_redirected)
			restore();
	}

	void redirect()
	{
		m_cerr_redirection_mutex_lock = std::unique_lock<std::mutex>(detail::cerr_redirection_mutex);
#ifndef NDEBUG
		m_cerr_buf_backup = std::cerr.rdbuf(m_string_stream.rdbuf());
#else
		class null_streambuf : public std::streambuf
		{
		public:
			int_type overflow(int_type c) override { return c; }
		};
		static null_streambuf null_buf;
		m_cerr_buf_backup = std::cerr.rdbuf(&null_buf);
#endif
		m_redirected = true;
	}

	void restore()
	{
		std::cerr.rdbuf(m_cerr_buf_backup);
		m_cerr_buf_backup = nullptr;
#ifndef NDEBUG
		std::string redirected_cerr = m_string_stream.str();
		source_location location = m_location;
#endif
		m_redirected = false;
		if (m_cerr_redirection_mutex_lock.owns_lock())
			m_cerr_redirection_mutex_lock.unlock();
#ifndef NDEBUG
		if (detail::is_logging_enabled() && !redirected_cerr.empty())
		{
			entry(location, std::make_tuple(stderr_redirect{}, serialization::object{{{"redirected_cerr", redirected_cerr}}}));
		}
#endif
	}

private:
	bool m_redirected{false};
	std::streambuf* m_cerr_buf_backup{nullptr};
#ifndef NDEBUG
	source_location m_location;
	std::ostringstream m_string_stream;
#endif
	std::unique_lock<std::mutex> m_cerr_redirection_mutex_lock;
};

} // namespace docwire::log

#endif // DOCWIRE_LOG_CERR_REDIRECTION_H
