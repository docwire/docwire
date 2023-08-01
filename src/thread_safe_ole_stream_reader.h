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

#ifndef DOCTOTEXT_THREAD_SAFE_OLE_STREAM_READER_H
#define DOCTOTEXT_THREAD_SAFE_OLE_STREAM_READER_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "wv2/olestream.h"
#include "defines.h"

class ThreadSafeOLEStorage;
using namespace wvWare;
class DataStream;

class DllExport ThreadSafeOLEStreamReader : public wvWare::AbstractOLEStreamReader
{
	friend class ThreadSafeOLEStorage;
	public:
		struct Implementation;
		Implementation* impl;
		struct Stream
		{
			uint64_t m_size;
			std::vector<uint32_t> m_file_positions;
			uint32_t m_sector_size;
			DataStream* m_data_stream;
		};
		ThreadSafeOLEStreamReader(ThreadSafeOLEStorage* storage, Stream& stream);
	public:
		~ThreadSafeOLEStreamReader() override;
		std::string getLastError() const;
		bool isValid() const override;
		int tell() const override;
		size_t size() const override;
		bool seek(int offset, int whence = SEEK_SET) override;
		bool readU8(U8& data);
		U8 readU8() override;
		bool readS8(S8& data);
		S8 readS8() override;
		bool readU16(U16& data);
		U16 readU16() override;
		bool readS16(S16& data);
		S16 readS16() override;
		bool readU32(U32& data);
		U32 readU32() override;
		bool readS32(S32& data);
		S32 readS32() override;
		bool read(U8 *buffer, size_t length) override;
};

#endif // DOCTOTEXT_THREAD_SAFE_OLE_STREAM_READER_H
