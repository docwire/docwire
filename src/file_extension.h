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

#ifndef DOCWIRE_FILE_EXTENSION_H
#define DOCWIRE_FILE_EXTENSION_H

#include <algorithm>
#include <filesystem>

namespace docwire
{

/**
 * @brief A class representing a file extension.
 *
 * File extensions are case-insensitive.
 */
class file_extension
{
public:
	/**
	 * @brief Constructs a file_extension object from a string.
	 *
	 * The string is converted to lowercase.
	 *
	 * @param ext The extension string.
	 */
	explicit file_extension(const std::string& ext) : m_ext(ext)
	{
		std::transform(m_ext.begin(), m_ext.end(), m_ext.begin(), [](char c) { return std::tolower(c); });
	}

	/**
	 * @brief Constructs a file_extension object from a path.
	 *
	 * The extension is extracted from the path and converted to lowercase.
	 *
	 * @param p The path.
	 */
	explicit file_extension(const std::filesystem::path& p) : m_ext(p.extension().string())
	{
		std::transform(m_ext.begin(), m_ext.end(), m_ext.begin(), [](char c) { return std::tolower(c); });
	}

	/**
	 * @brief Constructs a file_extension object from a const literal.
	 *
	 * The literal is converted to lowercase.
	 *
	 * @param ext The extension literal.
	 */
	explicit file_extension(const char* ext) : m_ext(ext)
	{
		std::transform(m_ext.begin(), m_ext.end(), m_ext.begin(), [](char c) { return std::tolower(c); });
	}


	/**
	 * @brief Returns the file extension as a string.
	 *
	 * @return The file extension.
	 */
	std::string string() const
	{
		return m_ext;
	}

	/**
	 * @brief Compares two file extensions for equality, ignoring case.
	 *
	 * @param other The file extension to compare with.
	 *
	 * @return True if the file extensions are equal, ignoring case; otherwise, false.
	 */
	bool operator==(const file_extension& other) const
	{
		return m_ext == other.m_ext;
	}

	/**
	 * @brief Compares two file extensions for equality, ignoring case.
	 *
	 * @param other The file extension to compare with.
	 *
	 * @return A comparison result indicating whether the file extensions are equal, less than, or greater than each other.
	 */
	std::strong_ordering operator<=>(const file_extension& other) const
	{
		#ifdef __cpp_lib_three_way_comparison
			return m_ext <=> other.m_ext;
		#else
			if (m_ext < other.m_ext)
				return std::strong_ordering::less;
			else if (m_ext > other.m_ext)
				return std::strong_ordering::greater;
			else
				return std::strong_ordering::equal;
		#endif			
	}

private:
	std::string m_ext;
};

} // namespace docwire

#endif
