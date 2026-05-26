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

#ifndef DOCWIRE_XML_ITERATOR_STATE_H
#define DOCWIRE_XML_ITERATOR_STATE_H

#include "xml_reader.h"

namespace docwire::xml
{

/**
 * @brief Shared state for XML iterators to coordinate traversal.
 * @tparam safety_level The safety policy.
 */
template <safety_policy safety_level>
struct iterator_state
{
	/**
	 * @brief Constructs the state with a reference to the XML reader.
	 */
	explicit iterator_state(reader<safety_level>& reader) : xml_reader(reader) {}
	/// Reference to the underlying XML reader.
	reader<safety_level>& xml_reader;

	/**
	 * @brief A shared flag indicating that the reader has been advanced one step ahead.
	 *
	 * This is a crucial part of the shared state for coordinating multiple iterators
	 * operating on the single underlying xml::reader cursor.
	 * When an iterator (like descendants_view::iterator) needs to "peek ahead" to check
	 * a stopping condition, it advances the reader and sets this flag. The next iterator
	 * to be incremented must check this flag, consume the current node without advancing
	 * the reader again, and then clear the flag. It MUST be part of the shared state.
	 */
	bool m_node_ahead_flag = false;
};

}

#endif
