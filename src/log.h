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

struct hex {};
struct begin_complex {};
struct end_complex {};
struct begin_pair { std::string key; };
struct end_pair {};

#define type_of(var) std::make_pair("typeid", std::type_index(typeid(var)))

class DllExport log_record_stream
{
public:
	log_record_stream(severity_level severity, source_location location);
	~log_record_stream();
	log_record_stream& operator<<(const char* msg);
	log_record_stream& operator<<(long int val);
	log_record_stream& operator<<(const std::string& str);
	log_record_stream& operator<<(const hex& h);
	log_record_stream& operator<<(const begin_complex&);
	log_record_stream& operator<<(const end_complex&);
	log_record_stream& operator<<(const std::type_index& t);
	log_record_stream& operator<<(const std::thread::id& i);
	log_record_stream& operator<<(severity_level severity);
	log_record_stream& operator<<(const begin_pair& b);
	log_record_stream& operator<<(const end_pair&);
	template<class T> log_record_stream& operator<<(const std::pair<const char*, T>& p)
	{
		*this << begin_pair{p.first} << p.second << end_pair();
		return *this;
	}
	log_record_stream& operator<<(const std::exception& e);
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
