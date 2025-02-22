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

#ifndef DOCWIRE_THREAD_SAFE_OLE_STORAGE_H
#define DOCWIRE_THREAD_SAFE_OLE_STORAGE_H

#include "core_export.h"
#include "pimpl.h"
#include <span>
#include <string>
#include <vector>
#include "wv2/olestorage.h"

namespace docwire
{

class ThreadSafeOLEStreamReader;
using namespace wvWare;

class DOCWIRE_CORE_EXPORT ThreadSafeOLEStorage : public AbstractOLEStorage, public with_pimpl<ThreadSafeOLEStorage>
{
	public:
		explicit ThreadSafeOLEStorage(const std::string& file_name);
		ThreadSafeOLEStorage(std::span<const std::byte> buffer);
		bool isValid() const override;
		bool open(Mode mode) override;
		void close() override;
		std::string name() const override;
		std::string getLastError();
		bool getStreamsAndStoragesList(std::vector<std::string>& components);
		bool enterDirectory(const std::string& directory_path);
		bool leaveDirectory();
		bool readDirectFromBuffer(unsigned char* buffer, int size, int offset) override;
		AbstractOLEStreamReader* createStreamReader(const std::string& stream_path) override;
	private:
		void streamDestroyed(OLEStream* stream) override;
};

} // namespace docwire

#endif // DOCWIRE_THREAD_SAFE_OLE_STORAGE_H
