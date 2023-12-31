/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "log.h"

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/json.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <stack>

namespace docwire
{

static std::atomic<severity_level> log_verbosity = info;

void set_log_verbosity(severity_level severity)
{
	log_verbosity = severity;
}

bool log_verbosity_includes(severity_level severity)
{
	return severity >= log_verbosity;
}

static std::atomic<std::ostream*> log_stream = &std::clog;

static std::atomic<bool> first_log_in_stream = true;

void set_log_stream(std::ostream* stream)
{
	log_stream = stream;
	first_log_in_stream = true;
}

struct log_record_stream::implementation
{
	boost::json::value root;
	std::stack<boost::json::value*> obj_stack;
	bool hex_numbers = false;
	implementation()
	{
		obj_stack.push(&root);
	}

	void insert_simple_value(const boost::json::value new_v)
	{
		boost::json::value& v = *obj_stack.top();
		if (v.is_null())
			v = new_v;
		else if (v.is_array())
			v.as_array().push_back(new_v);
		else
		{
			v = boost::json::array({ v });
			v.as_array().push_back(new_v);
		}
	}
};

namespace
{
	std::string normalize_type_name(const std::string& type_name)
	{
		std::string normalized = type_name;
		boost::algorithm::erase_all(normalized, "__cdecl ");
		boost::algorithm::erase_all(normalized, "__1::");
		boost::algorithm::erase_all(normalized, "virtual ");
		boost::algorithm::erase_all(normalized, "class ");
		boost::algorithm::erase_all(normalized, "struct ");
		boost::algorithm::replace_all(normalized, "(void)", "()");
		boost::algorithm::replace_all(normalized, ", ", ",");
		return normalized;
	}
} // anonymous namespace

log_record_stream::log_record_stream(severity_level severity, source_location location)
	: m_impl(new implementation())
{
	if (first_log_in_stream)
	{
		*log_stream << "[" << std::endl;
		first_log_in_stream = false;
	}
	else
		*log_stream << "," << std::endl;

	boost::posix_time::ptime utc_time = boost::posix_time::second_clock::universal_time();
	boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adjustor;
	boost::posix_time::ptime local_time = local_adjustor.utc_to_local(utc_time);
	boost::posix_time::time_duration timezone_offset = local_time - utc_time;
	long timezone_offset_seconds = timezone_offset.total_seconds();
	int timezone_offset_hours = timezone_offset_seconds / 3600;
	int timezone_offset_minutes = (timezone_offset_seconds % 3600) / 60;
	std::stringstream time_stream;
	time_stream
		<< boost::posix_time::to_iso_extended_string(local_time)
		<< std::setw(5) << std::setfill('0') << std::internal << std::showpos << timezone_offset_hours * 100 + timezone_offset_minutes;

	*this
		<< std::make_pair("timestamp", time_stream.str())
		<< std::make_pair("severity", severity)
		<< std::make_pair("file", std::filesystem::path(location.file_name).filename())
		<< std::make_pair("line", location.line)
		<< std::make_pair("function", normalize_type_name(location.function_name))
		<< std::make_pair("thread_id", std::this_thread::get_id())
		<< begin_pair{"log"};
}

log_record_stream::~log_record_stream()
{
	*this << end_pair();
	*log_stream << boost::json::serialize(m_impl->root);
}

log_record_stream& log_record_stream::operator<<(std::nullptr_t)
{
	boost::json::value new_v;
	m_impl->insert_simple_value(new_v);
	return *this;
}

log_record_stream& log_record_stream::operator<<(const char* msg)
{
	if (msg)
		m_impl->insert_simple_value(msg);
	else
		*this << nullptr;
	return *this;
}

log_record_stream& log_record_stream::operator<<(std::int64_t val)
{
	boost::json::value new_v;
	if (m_impl->hex_numbers)
	{
		std::ostringstream s;
		s << "0x" << std::hex << val;
		new_v = s.str().c_str();
	}
	else
		new_v = val;
	m_impl->insert_simple_value(new_v);
	return *this;
}

log_record_stream& log_record_stream::operator<<(std::uint64_t val)
{
	boost::json::value new_v;
	if (m_impl->hex_numbers)
	{
		std::ostringstream s;
		s << "0x" << std::hex << val;
		new_v = s.str().c_str();
	}
	else
		new_v = val;
	m_impl->insert_simple_value(new_v);
	return *this;
}

log_record_stream& log_record_stream::operator<<(double val)
{
	m_impl->insert_simple_value(val);
	return *this;
}

log_record_stream& log_record_stream::operator<<(bool val)
{
	m_impl->insert_simple_value(val);
	return *this;
}

log_record_stream& log_record_stream::operator<<(const std::string& str)
{
	m_impl->insert_simple_value(str.c_str());
	return *this;
}

log_record_stream& log_record_stream::operator<<(const begin_complex&)
{
	boost::json::value new_v;
	boost::json::value& v = *m_impl->obj_stack.top();
	if (v.is_null())
	{
		v = new_v;
		m_impl->obj_stack.push(m_impl->obj_stack.top());
	}
	else if (v.is_array())
	{
		v.as_array().push_back(new_v);
		m_impl->obj_stack.push(&v.as_array()[v.as_array().size() - 1]);
	}
	else
	{
		v = boost::json::array({ v });
		v.as_array().push_back(new_v);
		m_impl->obj_stack.push(&v.as_array()[v.as_array().size() - 1]);
	}
	return *this;
}

log_record_stream& log_record_stream::operator<<(const end_complex&)
{
	m_impl->obj_stack.pop();
	return *this;
}

log_record_stream& log_record_stream::operator<<(const hex& h)
{
	m_impl->hex_numbers = true;
	return *this;
}

log_record_stream& log_record_stream::operator<<(const std::type_index& t)
{
	*this << normalize_type_name(boost::core::demangle(t.name()));
	return *this;
}

log_record_stream& log_record_stream::operator<<(const std::thread::id& i)
{
	std::ostringstream s;
	s << i;
	*this << s.str();
	return *this;
}

log_record_stream& log_record_stream::operator<<(const std::filesystem::path& p)
{
	*this << p.string();
	return *this;
}

log_record_stream& log_record_stream::operator<<(severity_level severity)
{
	switch (severity)
	{
		case debug: *this << std::string("debug"); break;
		case info: *this << std::string("info"); break;
		case warning: *this << std::string("warning"); break;
		case error: *this << std::string("error"); break;
	}
	return *this;
}

log_record_stream& log_record_stream::operator<<(const begin_pair& b)
{
	boost::json::value& v = *m_impl->obj_stack.top();
	if (v.is_object())
	{
		v.as_object().emplace(b.key, boost::json::value());
		m_impl->obj_stack.push(&(v.as_object()[b.key]));
	}
	else
	{
		boost::json::value new_v = boost::json::object{{ b.key, boost::json::value() }};
		if (v.is_null())
		{
			v = new_v;
			m_impl->obj_stack.push(&(v.as_object()[b.key]));
		}
		else if (v.is_array())
		{
			v.as_array().push_back(new_v);
			m_impl->obj_stack.push(&(v.as_array()[v.as_array().size() - 1].as_object()[b.key]));
		}
		else
		{
			v = boost::json::array({ v });
			v.as_array().push_back(new_v);
			m_impl->obj_stack.push(&(v.as_array()[v.as_array().size() - 1].as_object()[b.key]));
		}
	}
	return *this;
}

log_record_stream& log_record_stream::operator<<(const end_pair&)
{
	*this << end_complex();
	return *this;
}

log_record_stream& log_record_stream::operator<<(const std::exception& e)
{
	*this << docwire_log_streamable_obj(e, e.what());
	return *this;
}

log_record_stream& log_record_stream::operator<<(const begin_array&)
{
	*this << begin_complex();
	boost::json::value& v = *m_impl->obj_stack.top();
	v = boost::json::array();
	return *this;
}

log_record_stream& log_record_stream::operator<<(const end_array&)
{
	*this << end_complex();
	return *this;
}

class Exiter
{
public:
	~Exiter()
	{
		if (!first_log_in_stream)
			*log_stream << std::endl << "]" << std::endl;
	}
};

static Exiter exiter;

static create_log_record_stream_func_t create_log_record_stream_func =
[](severity_level severity, source_location location) -> std::unique_ptr<log_record_stream>
{
	return std::make_unique<log_record_stream>(severity, location);
};

void set_create_log_record_stream_func(create_log_record_stream_func_t func)
{
	create_log_record_stream_func = func;
}

DllExport std::unique_ptr<log_record_stream> create_log_record_stream(severity_level severity, source_location location)
{
	return create_log_record_stream_func(severity, location);
}

static pthread_mutex_t cerr_log_redirection_mutex = PTHREAD_MUTEX_INITIALIZER;

struct cerr_log_redirection::implementation
{
	std::ostringstream string_stream;
};

cerr_log_redirection::cerr_log_redirection(source_location location)
	: m_redirected(false), m_cerr_buf_backup(nullptr), m_location(location), m_impl(new implementation())
{
	redirect();
}

cerr_log_redirection::~cerr_log_redirection()
{
	if (m_redirected)
		restore();
}

void cerr_log_redirection::redirect()
{
	if (log_verbosity_includes(debug))
	{
		pthread_mutex_lock(&cerr_log_redirection_mutex);
		m_cerr_buf_backup = std::cerr.rdbuf(m_impl->string_stream.rdbuf());
	}
	else
		std::cerr.setstate(std::ios::failbit);
	m_redirected = true;
}

void cerr_log_redirection::restore()
{
	if (m_cerr_buf_backup != nullptr)
	{
		std::cerr.rdbuf(m_cerr_buf_backup);
		pthread_mutex_unlock(&cerr_log_redirection_mutex);
		m_cerr_buf_backup = nullptr;
		if (!m_impl->string_stream.str().empty())
		{
			std::unique_ptr<log_record_stream> log_record_stream = create_log_record_stream(debug, m_location);
			*log_record_stream << m_impl->string_stream.str();
		}
	}
	else
		std::cerr.clear();
	m_redirected = false;
}

} // namespace docwire
