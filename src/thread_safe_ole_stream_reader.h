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

#ifndef DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H
#define DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H

#include <cstdint>
#include <cstdio>
#include "pimpl.h"
#include <string>
#include <vector>
#include "wv2/olestream.h"
#include "export.h"

namespace docwire
{

class ThreadSafeOLEStorage;
using namespace wvWare;
class DataStream;

class DOCWIRE_EXPORT ThreadSafeOLEStreamReader : public wvWare::AbstractOLEStreamReader, public with_pimpl<ThreadSafeOLEStreamReader>
{
	friend class ThreadSafeOLEStorage;
	public:
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

} // namespace docwire

#endif // DOCWIRE_THREAD_SAFE_OLE_STREAM_READER_H
