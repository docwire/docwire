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

#ifndef DOCWIRE_XML_READER_H
#define DOCWIRE_XML_READER_H

#include "safety_policy.h"
#include "pimpl.h"
#include <string_view>
#include "ranged.h"
#include "xml_export.h"

namespace docwire::xml
{
/**
 * @brief Options for handling blank nodes in the XML reader.
 */
enum class reader_blanks { keep, ignore };
/**
 * @brief Represents the type of an XML node.
 */
enum class node_type
{
	none = 0,
	element = 1,
	attribute = 2,
	text = 3,
	cdata = 4,
	entity_reference = 5,
	entity = 6,
	processing_instruction = 7,
	comment = 8,
	document = 9,
	document_type = 10,
	document_fragment = 11,
	notation = 12,
	whitespace = 13,
	significant_whitespace = 14,
	end_element = 15,
	end_entity = 16,
	xml_declaration = 17
};

/**
 * @brief A forward-only, non-cached XML reader.
 * 
 * This class provides a wrapper around libxml2's xmlTextReader, offering a modern C++ interface
 * with configurable safety policies.
 * 
 * @tparam safety_level The safety policy to use (strict or relaxed).
 * @warning This is a single-pass reader. Once the reader advances, previous nodes cannot be revisited.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class DOCWIRE_XML_EXPORT reader : public with_pimpl<reader<safety_level>>
{
	using with_pimpl<reader<safety_level>>::impl;
public:
	/**
	 * @brief Constructs a reader from a string view.
	 * @param xml_sv The XML content to parse.
	 * @param blanks_option Specifies whether to keep or ignore blank nodes (default: keep).
	 */
	explicit reader(std::string_view xml_sv, reader_blanks blanks_option = reader_blanks::keep);

	// Public low-level methods
	/**
	 * @brief Advances the reader to the next node.
	 * @return true if a node was read, false if the end of the document was reached.
	 */
	bool read_next() const;
	/**
	 * @brief Returns the content of the current node (e.g., text inside an element).
	 */
	std::string_view content() const;
	/**
	 * @brief Returns the local name of the current node.
	 */
	std::string_view name() const;
	/**
	 * @brief Returns the full name (including namespace) of the current node.
	 */
	std::string_view full_name() const;
	/**
	 * @brief Returns the string value of the current node (concatenated text of children).
	 */
	std::string_view string_value() const;
	// Attribute traversal methods
	/**
	 * @brief Moves the reader to the first attribute of the current element.
	 * @return true if successful, false if no attributes exist.
	 */
	bool move_to_first_attribute() const;
	/**
	 * @brief Moves the reader to the next attribute.
	 * @return true if successful, false if no more attributes exist.
	 */
	bool move_to_next_attribute() const;
	/**
	 * @brief Moves the reader back to the element containing the attributes.
	 * @throws docwire::error In strict mode, throws if the operation fails (e.g., reader is not positioned on an attribute).
	 */
	void move_to_element() const noexcept(safety_level == relaxed);

	/**
	 * @brief Returns the depth of the current node in the XML tree.
	 */
	non_negative<int, safety_level> depth() const;

	/**
	 * @brief Returns the type of the current node.
	 */
	node_type type() const;
};
}
#endif // DOCWIRE_XML_READER_H