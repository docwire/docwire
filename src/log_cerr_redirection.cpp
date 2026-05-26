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

#include "log_cerr_redirection.h"

#include <iostream>
#include "log_entry.h"
#include <mutex>
#include <sstream>

namespace docwire
{

namespace
{
	std::mutex cerr_redirection_mutex;
} // anonymous namespace

template<>
struct pimpl_impl<log::cerr_redirection> : pimpl_impl_base
{
	bool m_redirected = false;
	std::streambuf* m_cerr_buf_backup = nullptr;
	#ifndef NDEBUG
		source_location m_location;
		std::ostringstream string_stream;
	#endif
	std::unique_lock<std::mutex> cerr_redirection_mutex_lock;
};

namespace log
{

cerr_redirection::cerr_redirection(const source_location& location)
{
	#ifndef NDEBUG
		impl().m_location = location;
	#endif
	redirect();
}

cerr_redirection::~cerr_redirection()
{
	if (impl().m_redirected)
		restore();
}

void cerr_redirection::redirect()
{
	impl().cerr_redirection_mutex_lock = std::unique_lock<std::mutex>(cerr_redirection_mutex);
	#ifndef NDEBUG
		impl().m_cerr_buf_backup = std::cerr.rdbuf(impl().string_stream.rdbuf());
	#else
		class null_streambuf : public std::streambuf
		{
			public:
				int_type overflow(int_type c) override { return c; }
		};
		static null_streambuf null_buf;
		impl().m_cerr_buf_backup = std::cerr.rdbuf(&null_buf);
	#endif
	impl().m_redirected = true;
}

void cerr_redirection::restore()
{
	std::cerr.rdbuf(impl().m_cerr_buf_backup);
	impl().m_cerr_buf_backup = nullptr;
#ifndef NDEBUG
	std::string redirected_cerr = impl().string_stream.str();
	source_location location = impl().m_location;
#endif
	impl().m_redirected = false;
	impl().cerr_redirection_mutex_lock.unlock();
#ifndef NDEBUG
	if (detail::is_logging_enabled() && !redirected_cerr.empty())
	{
		log::entry(location, std::make_tuple(log::stderr_redirect{}, serialization::object{{{"redirected_cerr", redirected_cerr}}}));
	}
#endif
}

} // namespace log

} // namespace docwire
