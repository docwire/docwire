#ifndef DOCWIRE_AUDIT_STDERR_REDIRECTION_HPP
#define DOCWIRE_AUDIT_STDERR_REDIRECTION_HPP

#include "audit_export.h"
#include "auditor_ref.hpp"
#include "entry.hpp"
#include "../serialization_base.h"
#include "../source_location.h"

#include <iostream>
#include <mutex>
#include <sstream>
#include <streambuf>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace docwire::audit
{

/// @brief Tag for audit entries that contain content redirected from `stderr`.
struct stderr_redirect
{
    static constexpr std::string_view string() { return "stderr_redirect"; }
};

namespace detail
{

/**
 * @brief Returns the process-wide mutex protecting `std::cerr` redirection.
 *
 * The mutex lives in `docwire_audit` so it can be shared across DSO boundaries
 * without exposing global mutable state in the header.
 */
DOCWIRE_AUDIT_EXPORT std::mutex& cerr_redirection_mutex();

} // namespace detail

/**
 * @brief RAII helper that captures everything written to `std::cerr` while active.
 *
 * The captured text is reported through the injected auditor when the redirection
 * ends (either via `restore()` or destruction). In release builds the captured
 * content is discarded and `std::cerr` is redirected to a null stream.
 *
 * @tparam auditor_type The auditor type used to report the captured stderr.
 */
template<typename auditor_type = ::docwire::audit::null_auditor>
class stderr_redirection
{
public:
	explicit stderr_redirection(
		::docwire::audit::auditor_ref<auditor_type> auditor = {},
		const ::docwire::source_location& location = ::docwire::source_location::current())
		: m_auditor{auditor}
	{
#ifndef NDEBUG
		m_location = location;
#endif
		redirect();
	}

	stderr_redirection(const stderr_redirection&) = delete;
	stderr_redirection& operator=(const stderr_redirection&) = delete;

	~stderr_redirection()
	{
		if (m_redirected)
			restore();
	}

	void redirect()
	{
		m_cerr_redirection_mutex_lock =
			std::unique_lock<std::mutex>(detail::cerr_redirection_mutex());
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
		m_redirected = false;
		if (m_cerr_redirection_mutex_lock.owns_lock())
			m_cerr_redirection_mutex_lock.unlock();
#ifndef NDEBUG
		std::string redirected_cerr = m_string_stream.str();
		m_string_stream.str(std::string{});
		m_string_stream.clear();

		if (!redirected_cerr.empty())
		{
			::docwire::audit::entry(
				m_auditor,
				[&] {
					return serialization::object{
						{{"redirected_cerr", redirected_cerr}}
					};
				},
				stderr_redirect{},
				m_location);
		}
#endif
	}

private:
	bool m_redirected{false};
	std::streambuf* m_cerr_buf_backup{nullptr};
	::docwire::audit::auditor_ref<auditor_type> m_auditor;
#ifndef NDEBUG
	source_location m_location;
	std::ostringstream m_string_stream;
#endif
	std::unique_lock<std::mutex> m_cerr_redirection_mutex_lock;
};

} // namespace docwire::audit

#endif // DOCWIRE_AUDIT_STDERR_REDIRECTION_HPP
