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
#include "minizip_wrapper.h"
#include <string>

namespace docwire
{

class zip_reader
{
public:
	explicit zip_reader(const data_source& data)
		: m_state{data.span(), 0, 0, {}, false}
	{
	}

	~zip_reader()
	{
		detail::minizip::close(m_state);
	}

	void open()
	{
		detail::minizip::open(m_state, m_state.source);
	}

	bool exists(const std::string& file_name) const
	{
		return detail::minizip::exists(const_cast<detail::minizip::state&>(m_state), file_name);
	}

	bool read(const std::string& file_name, std::string* contents, int num_of_chars = 0)
	{
		return detail::minizip::read(m_state, file_name, contents, num_of_chars);
	}

	bool getFileSize(const std::string& file_name, unsigned long& file_size)
	{
		return detail::minizip::get_file_size(m_state, file_name, file_size);
	}

	bool readChunk(const std::string& file_name, std::string* contents, int num_of_chars)
	{
		return detail::minizip::read_chunk(m_state, file_name, contents, num_of_chars);
	}

	bool readChunk(const std::string& file_name, char* contents, int num_of_chars, int& readed, bool add_null_terminator = true)
	{
		return detail::minizip::read_chunk(m_state, file_name, contents, num_of_chars, readed, add_null_terminator);
	}

	void closeReadingFileForChunks()
	{
		detail::minizip::close_reading_file_for_chunks(m_state);
	}

	bool loadDirectory()
	{
		return detail::minizip::load_directory(m_state);
	}

private:
	detail::minizip::state m_state;
};

}; // namespace docwire

#endif
