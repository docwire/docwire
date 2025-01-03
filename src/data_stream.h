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

#ifndef DOCWIRE_DATA_STREAM_H
#define DOCWIRE_DATA_STREAM_H

#include <stdio.h>
#include <string>
#include "defines.h"
#include "pimpl.h"

namespace docwire
{

class DataStream
{
	public:
		virtual ~DataStream(){}
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
		virtual DataStream* clone() = 0;
};

class DllExport FileStream : public DataStream, public with_pimpl<FileStream>
{
	public:
		FileStream(const std::string& file_name);
		~FileStream();
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
		DataStream* clone();
};

class DllExport BufferStream : public DataStream, public with_pimpl<BufferStream>
{
	public:
		BufferStream(const char* buffer, size_t size);
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
		DataStream* clone();
};

} // namespace docwire

#endif	//DOCWIRE_DATA_STREAM_H
