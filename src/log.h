/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
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
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
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

#ifndef DOCTOTEXT_LOG_H
#define DOCTOTEXT_LOG_H

#include "defines.h"
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <typeindex>

namespace doctotext
{

enum severity_level
{
	debug,
	info,
	warning,
	error
};

struct DllExport source_location
{
	std::string file_name;
	unsigned int line;
	std::string function_name;
};

DllExport void set_log_verbosity(severity_level severity);

DllExport bool log_verbosity_includes(severity_level severity);

DllExport void set_log_stream(std::ostream* stream);

struct DllExport hex {};
struct DllExport begin_complex {};
struct DllExport end_complex {};
struct DllExport begin_pair { std::string key; };
struct DllExport end_pair {};
struct DllExport begin_array {};
struct DllExport end_array {};

#define doctotext_log_type_of(var) std::make_pair("typeid", std::type_index(typeid(var)))

class DllExport log_record_stream
{
public:
	log_record_stream(severity_level severity, source_location location);
	~log_record_stream();
	log_record_stream& operator<<(const char* msg);
	log_record_stream& operator<<(std::int64_t val);
	log_record_stream& operator<<(std::uint64_t val);
	log_record_stream& operator<<(double val);
	log_record_stream& operator<<(bool val);
	template<typename T> typename std::enable_if<std::is_signed_v<T>, log_record_stream&>::type operator<<(T val)
	{
		*this << (std::int64_t)val;
		return *this;
	}
	template<typename T> typename std::enable_if<std::is_unsigned_v<T>, log_record_stream&>::type operator<<(T val)
	{
		*this << (std::uint64_t)val;
		return *this;
	}
	log_record_stream& operator<<(const std::string& str);
	log_record_stream& operator<<(const hex& h);
	log_record_stream& operator<<(const begin_complex&);
	log_record_stream& operator<<(const end_complex&);
	log_record_stream& operator<<(const std::type_index& t);
	log_record_stream& operator<<(const std::thread::id& i);
	log_record_stream& operator<<(const std::filesystem::path& p);
	log_record_stream& operator<<(severity_level severity);
	log_record_stream& operator<<(const begin_pair& b);
	log_record_stream& operator<<(const end_pair&);
	template<class T> log_record_stream& operator<<(const std::pair<const char*, T>& p)
	{
		*this << begin_pair{p.first} << p.second << end_pair();
		return *this;
	}
	log_record_stream& operator<<(const std::exception& e);
	log_record_stream& operator<<(const begin_array&);
	log_record_stream& operator<<(const end_array&);
	template<class T> log_record_stream& operator<<(const std::vector<T>& v)
	{
		*this << begin_array();
		for (auto i: v)
			*this << i;
		*this << end_array();
		return *this;
	}
	template<typename T>
	typename std::enable_if<std::is_member_function_pointer_v<decltype(&T::log_to_record_stream)>, log_record_stream&>::type
	operator<<(const T& v)
	{
		v.log_to_record_stream(*this);
		return *this;
	}

	template<typename T> log_record_stream& operator<<(const T* pointer)
	{
		if (pointer)
			*this << begin_complex() << doctotext_log_type_of(pointer) << std::make_pair("dereferenced", std::cref(*pointer)) << end_complex();
		else
			*this << nullptr;
		return *this;
	}

private:
	struct implementation;
	std::unique_ptr<implementation> m_impl;
};

typedef std::function<std::unique_ptr<log_record_stream>(severity_level severity, source_location location)> create_log_record_stream_func_t;

DllExport void set_create_log_record_stream_func(create_log_record_stream_func_t func);

DllExport std::unique_ptr<log_record_stream> create_log_record_stream(severity_level severity, source_location location);

inline void current_function_helper()
{
#if defined(__GNUC__)
#define doctotext_current_function __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define doctotext_current_function __FUNCSIG__
#else
#define doctotext_current_fuction __func__
#endif
}

#define doctotext_current_source_location() \
	doctotext::source_location{__FILE__, __LINE__, doctotext_current_function}

#define doctotext_log(severity) \
	if (!doctotext::log_verbosity_includes(severity)) \
	{ \
	} \
	else \
		(*doctotext::create_log_record_stream(severity, doctotext_current_source_location()))

#define doctotext_log_named_variable(v) std::make_pair(#v, v)
#define doctotext_log_vars_args_count_helper(a1, a2, a3, a4, a5, a6, a7, a8, count, ...) count
#define doctotext_log_vars_args_count(...) doctotext_log_vars_args_count_helper(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define doctotext_log_vars_concatenate(a, b) doctotext_log_vars_concatenate_2(a, b)
#define doctotext_log_vars_concatenate_2(a, b) a##b
#define doctotext_log_vars_1(v) << doctotext_log_named_variable(v)
#define doctotext_log_vars_2(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_1(__VA_ARGS__)
#define doctotext_log_vars_3(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_2(__VA_ARGS__)
#define doctotext_log_vars_4(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_3(__VA_ARGS__)
#define doctotext_log_vars_5(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_4(__VA_ARGS__)
#define doctotext_log_vars_6(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_5(__VA_ARGS__)
#define doctotext_log_vars_7(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_6(__VA_ARGS__)
#define doctotext_log_vars_8(v, ...) doctotext_log_vars_1(v) doctotext_log_vars_7(__VA_ARGS__)
#define doctotext_log_vars_impl(...) doctotext_log_vars_concatenate(doctotext_log_vars_, doctotext_log_vars_args_count(__VA_ARGS__))(__VA_ARGS__)
#define doctotext_log_vars(...) doctotext_log(debug) doctotext_log_vars_impl(__VA_ARGS__)
#define doctotext_log_var(v) doctotext_log_vars(v)

#define doctotext_log_func() doctotext_log(debug) << "Entering function" << std::make_pair("funtion_name", doctotext_current_function)
#define doctotext_log_func_with_args(...) doctotext_log_func() doctotext_log_vars_impl(__VA_ARGS__)

class DllExport cerr_log_redirection
{
public:
	cerr_log_redirection(source_location location);
	~cerr_log_redirection();
	void redirect();
	void restore();

private:
	bool m_redirected;
	struct implementation;
	std::unique_ptr<implementation> m_impl;
	std::streambuf* m_cerr_buf_backup;
	source_location m_location;
};

} // namespace doctotext

#endif
