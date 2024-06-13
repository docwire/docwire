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

#ifndef DOCWIRE_MEMORY_STREAM_H
#define DOCWIRE_MEMORY_STREAM_H

#include <istream>
#include <span>

namespace docwire
{

/**
 * @brief imemorystreambuf is a stream buffer that wraps std::span<const std::byte> and provides a compatible interface with std::istream.
 *
 * C++20 introduced std::span, which is a utility class that provides a way to access a contiguous sequence of elements.
 * imemorystreambuf is a stream buffer that wraps std::span<const std::byte> and provides a compatible interface with std::istream.
 *
 * Unlike std::istringstream, imemorystreambuf does not make a copy of the underlying data.
 * It uses a std::span to provide a way to access the data.
 *
 * There are several alternatives to using imemorystreambuf, such as std::istringstream, but they have a few limitations:
 * - std::istringstream requires a copy of the data.
 * - std::istrstream is deprecated in C++11 and removed in C++26.
 * - std::ispanstream but it requires C++23
 *
 * imemorystreambuf is a drop-in replacement for std::istrstream and is compatible with C++20 and later.
 */
class imemorystreambuf : public std::streambuf
{
	std::span<const std::byte> m_source;

public:
	/**
	 * @brief Construct a new imemorystreambuf object
	 *
	 * @param source The source data to be used by the stream buffer.
	 */
	explicit imemorystreambuf(std::span<const std::byte> source) :
		m_source(source)
	{
		char* p = const_cast<char*>(reinterpret_cast<const char*>(m_source.data()));
		setg(p, p, p + m_source.size());
	}

	/**
	 * @brief Set the position of the stream buffer.
	 *
	 * @param sp The position to set.
	 * @param which The I/O mode.
	 * @return std::streampos The new position of the stream buffer.
	 */
	std::streampos seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in) override
	{
		if (which != std::ios_base::in || sp < 0 || sp > m_source.size())
			return -1;
		char* p = const_cast<char*>(reinterpret_cast<const char*>(m_source.data()));
		setg(p, p + sp, p + m_source.size());
		return sp;
	}

	/**
	 * @brief Move the position of the stream buffer.
	 *
	 * @param off The offset to move the position.
	 * @param way The direction to move the position.
	 * @param which The I/O mode.
	 * @return std::streampos The new position of the stream buffer.
	 */
	std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in) override
	{
        if (which != std::ios_base::in)
            return -1;
		switch (way)
		{
			case std::ios_base::beg:
				return seekpos(off, which);
			case std::ios_base::cur:
				return seekpos(gptr() - eback() + off, which);
			case std::ios_base::end:
				return seekpos(m_source.size() + off , which);
			default:
				return -1;
		}
	}
};

/**
 * @brief imemorystream is a stream that wraps a std::span of const std::byte and provides a compatible interface with std::istream.
 *
 * C++20 introduced std::span, which is a utility class that provides a way to access a contiguous sequence of elements.
 * imemorystream is a stream that wraps std::span<const std::byte> and provides a compatible interface with std::istream.
 *
 * Unlike std::istringstream, imemorystream does not make a copy of the underlying data.
 * It uses a std::span to provide a way to access the data.
 *
 * There are several alternatives to using imemorystream, such as std::istringstream, but they have a few limitations:
 * - std::istringstream requires a copy of the data.
 * - std::istrstream is deprecated in C++11 and removed in C++26.
 * - std::ispanstream but it requires C++23
 *
 * imemorystream is a drop-in replacement for std::istrstream and is compatible with C++20 and later.
 */
class imemorystream : public std::istream
{
public:
	/**
	 * @brief Construct a new imemorystream object
	 *
	 * @param source The source data to be used by the stream.
	 */
	explicit imemorystream(std::span<const std::byte> source)
        : std::istream(&m_streambuf), m_streambuf(source)
	{}

private:
	/**
	 * @brief The stream buffer that wraps the source data.
	 */
	imemorystreambuf m_streambuf;
};

} // namespace docwire

#endif // DOCWIRE_MEMORY_STREAM_H
