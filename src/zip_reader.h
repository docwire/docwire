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

#ifndef DOCWIRE_ZIP_READER_H
#define DOCWIRE_ZIP_READER_H

#include "core_export.h"
#include "data_source.h"
#include <string>
#include "pimpl.h"

namespace docwire
{

class DOCWIRE_CORE_EXPORT zip_reader : public with_pimpl<zip_reader>
{
	public:
		zip_reader(const data_source& data);
		~zip_reader();
		void open();
		bool exists(const std::string& file_name) const;
		bool read(const std::string& file_name, std::string* contents, int num_of_chars = 0);
		bool getFileSize(const std::string& file_name, unsigned long& file_size);
		bool readChunk(const std::string& file_name, std::string* contents, int num_of_chars);
		bool readChunk(const std::string& file_name, char* contents, int num_of_chars, int& readed, bool add_null_terminator = true);
		void closeReadingFileForChunks();
		/**
			Load and cache zip file directory. Speed up locating files dramatically. Use before multiple read() calls.
		**/
		bool loadDirectory();
};

}; // namespace docwire

#endif
