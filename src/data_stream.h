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

#ifndef DOCWIRE_DATA_STREAM_H
#define DOCWIRE_DATA_STREAM_H

#include "core_export.h"
#include <stdio.h>
#include <string>
#include "pimpl.h"

namespace docwire
{

class data_stream
{
	public:
		virtual ~data_stream(){}
		virtual bool open() = 0;
		virtual bool close() = 0;
		virtual bool read(void* data, int element_size, size_t elements_num) = 0;
		virtual bool seek(int offset, int whence) = 0;
		virtual bool eof() = 0;
		virtual int getc() = 0;
		virtual bool unGetc(int ch) = 0;
		virtual size_t size() = 0;
		virtual size_t tell() = 0;
		virtual std::string name() = 0;
		virtual data_stream* clone() = 0;
};

class DOCWIRE_CORE_EXPORT file_stream : public data_stream, public with_pimpl<file_stream>
{
	public:
		file_stream(const std::string& file_name);
		~file_stream();
		bool open();
		bool close();
		bool read(void* data, int element_size, size_t elements_num);
		bool seek(int offset, int whence);
		bool eof();
		int getc();
		bool unGetc(int ch);
		size_t size();
		size_t tell();
		std::string name();
		data_stream* clone();
};

class DOCWIRE_CORE_EXPORT buffer_stream : public data_stream, public with_pimpl<buffer_stream>
{
	public:
		buffer_stream(const char* buffer, size_t size);
		bool open();
		bool close();
		bool read(void* data, int element_size, size_t elements_num);
		bool seek(int offset, int whence);
		bool eof();
		int getc();
		bool unGetc(int ch);
		size_t size();
		size_t tell();
		std::string name();
		data_stream* clone();
};

} // namespace docwire

#endif	//DOCWIRE_DATA_STREAM_H
