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

#ifndef DOCWIRE_UNIQUE_IDENTIFIER_H
#define DOCWIRE_UNIQUE_IDENTIFIER_H

#include <atomic>

namespace docwire
{

/**
 * @brief The class represents unique (for a single program run) identifier of an object.
 *
 * The class has a default constructor that generates new unique identifiers. Generation, copying and comparison
 * of identifiers should be very fast. Generation is thread-safe, which means that identifiers will not be duplicated
 * between different threads.
 */
class unique_identifier
{
public:
	/**
	 * @brief Default constructor that generates new unique identifier.
	 */
	unique_identifier() : m_id(m_counter++) {}

	/**
	 * @brief Copy constructor.
	 */
	unique_identifier(const unique_identifier&) = default;

	/**
	 * @brief Copy assignment operator.
	 */
	unique_identifier& operator=(const unique_identifier&) = default;

	/**
	 * @brief Compare two unique identifiers.
	 *
	 * @param other Identifier to compare with.
	 * @return Strong ordering between this and other identifiers.
	 */
	std::strong_ordering operator<=>(const unique_identifier& other) const { return m_id <=> other.m_id; }

    bool operator==(const unique_identifier& other) const = default;

private:
	inline static std::atomic<size_t> m_counter{0};  ///< Counter used for generating unique identifiers.
	size_t m_id;                                     ///< Unique identifier value.

	friend struct std::hash<unique_identifier>;
};

} // namespace docwire

namespace std {
template<>
struct hash<docwire::unique_identifier> {
	size_t operator()(const docwire::unique_identifier& id) const {
		return std::hash<size_t>{}(id.m_id);
	}
};
} // namespace std

#endif // DOCWIRE_UNIQUE_IDENTIFIER_H
