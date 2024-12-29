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

#ifndef DOCWIRE_LOG_H
#define DOCWIRE_LOG_H

#include "defines.h"
#include "pimpl.h"
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <typeindex>

namespace docwire
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

#define docwire_log_streamable_type_of(var) std::make_pair("typeid", std::type_index(typeid(var)))

template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>()), std::end(std::declval<T>()))>> : std::true_type {};

template <typename T, typename = void>
struct is_dereferenceable : std::false_type {};

template <typename T>
struct is_dereferenceable<T, std::void_t<decltype(*std::declval<T>()),
                                         decltype(bool(!std::declval<T>()))>> : std::true_type {};

class DllExport log_record_stream : public with_pimpl<log_record_stream>
{
public:
	log_record_stream(severity_level severity, source_location location);
	~log_record_stream();
	log_record_stream& operator<<(std::nullptr_t);
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
	template<typename T1, typename T2, typename = std::enable_if_t<std::is_convertible_v<T1, std::string_view>>> log_record_stream& operator<<(const std::pair<T1, T2>& p)
	{
		*this << begin_pair{p.first} << p.second << end_pair();
		return *this;
	}
	log_record_stream& operator<<(const std::exception& e);
	log_record_stream& operator<<(const begin_array&);
	log_record_stream& operator<<(const end_array&);

	template<class T, typename std::enable_if_t<is_iterable<T>::value, bool> = true>
	log_record_stream& operator<<(const T& v)
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

	template<typename T, typename std::enable_if_t<is_dereferenceable<T>::value, bool> = true>
	log_record_stream& operator<<(const T& dereferenceable)
	{
		if (dereferenceable)
			*this << begin_complex() << docwire_log_streamable_type_of(dereferenceable) << std::make_pair("dereferenced", std::cref(*dereferenceable)) << end_complex();
		else
			*this << nullptr;
		return *this;
	}
};

typedef std::function<std::unique_ptr<log_record_stream>(severity_level severity, source_location location)> create_log_record_stream_func_t;

DllExport void set_create_log_record_stream_func(create_log_record_stream_func_t func);

DllExport std::unique_ptr<log_record_stream> create_log_record_stream(severity_level severity, source_location location);

inline void current_function_helper()
{
#if defined(__GNUC__)
#define docwire_current_function __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define docwire_current_function __FUNCSIG__
#else
#define docwire_current_function __func__
#endif
}

#define docwire_current_source_location() \
	docwire::source_location{__FILE__, __LINE__, docwire_current_function}

#define docwire_log(severity) \
	if (!docwire::log_verbosity_includes(severity)) \
	{ \
	} \
	else \
		(*docwire::create_log_record_stream(severity, docwire_current_source_location()))

inline std::string prepare_var_name(const std::string& var_name)
{
	size_t pos = var_name.find('.');
	return (pos == std::string::npos ? var_name : var_name.substr(pos + 1));
}

template<typename T> std::pair<std::string, const T&> streamable_var(const std::string& var_name, T&& var)
{
	return std::pair<std::string, const T&>{prepare_var_name(var_name), var};
}

#define docwire_log_streamable_var(v) streamable_var(#v, v)

#define docwire_log_args_count_helper( \
	a01, a02, a03, a04, a05, a06, a07, a08, a09, a10, a11, a12, a13, a14, a15, a16, \
	a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, \
	count, ...) count
#define docwire_log_args_count(...) \
	docwire_log_args_count_helper(__VA_ARGS__, \
		32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
		16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define docwire_log_concatenate(a, b) docwire_log_concatenate_2(a, b)
#define docwire_log_concatenate_2(a, b) a##b
#define docwire_log_streamable_vars_1(v) docwire_log_streamable_var(v)
#define docwire_log_streamable_vars_2(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_1(__VA_ARGS__)
#define docwire_log_streamable_vars_3(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_2(__VA_ARGS__)
#define docwire_log_streamable_vars_4(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_3(__VA_ARGS__)
#define docwire_log_streamable_vars_5(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_4(__VA_ARGS__)
#define docwire_log_streamable_vars_6(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_5(__VA_ARGS__)
#define docwire_log_streamable_vars_7(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_6(__VA_ARGS__)
#define docwire_log_streamable_vars_8(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_7(__VA_ARGS__)
#define docwire_log_streamable_vars_9(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_8(__VA_ARGS__)
#define docwire_log_streamable_vars_10(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_9(__VA_ARGS__)
#define docwire_log_streamable_vars_11(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_10(__VA_ARGS__)
#define docwire_log_streamable_vars_12(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_11(__VA_ARGS__)
#define docwire_log_streamable_vars_13(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_12(__VA_ARGS__)
#define docwire_log_streamable_vars_14(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_13(__VA_ARGS__)
#define docwire_log_streamable_vars_15(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_14(__VA_ARGS__)
#define docwire_log_streamable_vars_16(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_15(__VA_ARGS__)
#define docwire_log_streamable_vars_17(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_16(__VA_ARGS__)
#define docwire_log_streamable_vars_18(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_17(__VA_ARGS__)
#define docwire_log_streamable_vars_19(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_18(__VA_ARGS__)
#define docwire_log_streamable_vars_20(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_19(__VA_ARGS__)
#define docwire_log_streamable_vars_21(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_20(__VA_ARGS__)
#define docwire_log_streamable_vars_22(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_21(__VA_ARGS__)
#define docwire_log_streamable_vars_23(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_22(__VA_ARGS__)
#define docwire_log_streamable_vars_24(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_23(__VA_ARGS__)
#define docwire_log_streamable_vars_25(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_24(__VA_ARGS__)
#define docwire_log_streamable_vars_26(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_25(__VA_ARGS__)
#define docwire_log_streamable_vars_27(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_26(__VA_ARGS__)
#define docwire_log_streamable_vars_28(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_27(__VA_ARGS__)
#define docwire_log_streamable_vars_29(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_28(__VA_ARGS__)
#define docwire_log_streamable_vars_30(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_29(__VA_ARGS__)
#define docwire_log_streamable_vars_31(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_30(__VA_ARGS__)
#define docwire_log_streamable_vars_32(v, ...) docwire_log_streamable_vars_1(v) << docwire_log_streamable_vars_31(__VA_ARGS__)

#define docwire_log_streamable_vars(...) docwire_log_concatenate(docwire_log_streamable_vars_, docwire_log_args_count(__VA_ARGS__))(__VA_ARGS__)
#define docwire_log_vars(...) docwire_log(debug) << docwire_log_streamable_vars(__VA_ARGS__)
#define docwire_log_var(v) docwire_log_vars(v)

#define docwire_log_streamable_obj(obj, ...) \
	begin_complex() << docwire_log_streamable_type_of(obj) << docwire_log_streamable_vars(__VA_ARGS__) << end_complex()

#define docwire_log_func() docwire_log(debug) << "Entering function" << std::make_pair("funtion_name", docwire_current_function)
#define docwire_log_func_with_args(...) docwire_log_func() << docwire_log_streamable_vars(__VA_ARGS__)

class DllExport cerr_log_redirection : public with_pimpl<cerr_log_redirection>
{
public:
	cerr_log_redirection(source_location location);
	~cerr_log_redirection();
	void redirect();
	void restore();

private:
	bool m_redirected;
	std::streambuf* m_cerr_buf_backup;
	source_location m_location;
};

} // namespace docwire

#endif
