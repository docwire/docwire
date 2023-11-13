/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_ZIP_READER_H
#define DOCWIRE_ZIP_READER_H

#include <string>
#include "defines.h"

namespace docwire
{

class ZipReader
{
	private:
		struct Implementation;
		Implementation* Impl;

	public:
		ZipReader();
		ZipReader(const std::string& archive_file_name);
		ZipReader(const char* buffer, size_t size);
		void setArchiveFile(const std::string& archive_file_name);
		void setBuffer(const char* buffer, size_t size);
		~ZipReader();
		DllExport static void setUnzipCommand(const std::string& command);
		bool open();
		void close();
		bool exists(const std::string& file_name) const;
		bool read(const std::string& file_name, std::string* contents, int num_of_chars = 0) const;
		bool getFileSize(const std::string& file_name, unsigned long& file_size) const;
		bool readChunk(const std::string& file_name, std::string* contents, int chunk_size) const;
		bool readChunk(const std::string& file_name, char* contents, int chunk_size, int& readed) const;
		void closeReadingFileForChunks() const;
		/**
			Load and cache zip file directory. Speed up locating files dramatically. Use before multiple read() calls.
		**/
		bool loadDirectory();
};

}; // namespace docwire

#endif
