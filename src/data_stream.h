/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
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

#ifndef DOCTOTEXT_DATA_STREAM_H
#define DOCTOTEXT_DATA_STREAM_H

#include <stdio.h>
#include <string>
#include "defines.h"

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

class DllExport FileStream : public DataStream
{
	struct Implementation;
	Implementation* impl;
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

class DllExport BufferStream : public DataStream
{
	struct Implementation;
	Implementation* impl;
	public:
		BufferStream(const char* buffer, size_t size);
		~BufferStream();
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

#endif	//DOCTOTEXT_DATA_STREAM_H
