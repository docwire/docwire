/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  https://github.com/docwire/docwire                                                                                                             */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include "log.h"

#include <boost/core/demangle.hpp>
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
	std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::ostringstream time_stream;
	time_stream << std::put_time(std::localtime(&t), "%FT%T%z");
	*this
		<< std::make_pair("timestamp", time_stream.str())
		<< std::make_pair("severity", severity)
		<< std::make_pair("file", std::filesystem::path(location.file_name).filename())
		<< std::make_pair("line", location.line)
		<< std::make_pair("function", location.function_name)
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
	m_impl->insert_simple_value(nullptr);
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
	*this << boost::core::demangle(t.name());
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
