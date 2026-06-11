/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/* *******************************************************************************************************************************************/

#ifndef DOCWIRE_XML_NODES_VIEW_H
#define DOCWIRE_XML_NODES_VIEW_H

#include <optional>
#include "checked.h"
#include <ranges>
#include "sentinel.h"
#include "xml_node_ref.h"
#include "xml_iterator_state.h"
#include "xml_reader.h"

namespace docwire::xml
{

/**
 * @brief A view over a sequence of XML nodes.
 * @tparam safety_level The safety policy.
 * @warning This view relies on a single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<safety_policy safety_level = default_safety_level>
class nodes_view : public std::ranges::view_base
{
public:
	class iterator;
	iterator begin() const { return iterator{m_state}; }
	sentinel end() const { return {}; }

	/**
	 * @brief Constructs a view from an iterator state.
	 * @param state The shared iterator state.
	 */
	explicit nodes_view(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state)
		: m_state(std::move(state)) {}
private:
	not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
};

/**
 * @brief Iterator for traversing a sequence of XML nodes.
 */
template<safety_policy safety_level>
class nodes_view<safety_level>::iterator
{
	friend class nodes_view<safety_level>;

public:
	using iterator_concept  = std::input_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = node_ref<safety_level>;
	using pointer           = const value_type*;
	using reference         = const value_type&;

	bool operator==(const sentinel& s) const { return !m_node.has_value(); }
	reference operator*() const { return *m_node; }
	pointer operator->() const { return &*m_node; }
	iterator& operator++()
	{
		if (!m_state->xml_reader.read_next())
		{
			m_node.reset();
			return *this;
		}
		m_node.emplace(m_state);
		return *this;
	}
	void operator++(int)
	{
		++(*this);
	}
	/**
	 * @brief Resets the iterator to the end state (invalidates it).
	 */
	void reset() { m_node.reset(); }

private:
	explicit iterator(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state)
		: m_state(std::move(state))
	{
		++(*this); // Advance to the first node upon construction
	}
	not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
	checked<std::optional<node_ref<safety_level>>, safety_level> m_node;
};

/**
 * @brief Creates a view of nodes starting from the given node's state.
 * @tparam safety_level The safety policy.
 * @param node The reference node.
 * @warning This function relies on a single-pass XML reader.
 */
template<safety_policy safety_level>
nodes_view<safety_level> nodes(const node_ref<safety_level>& node)
{
	return nodes_view<safety_level>{node.state()};
}

/**
 * @brief Creates a view of nodes from the reader.
 * @tparam safety_level The safety policy.
 * @param reader The XML reader.
 * @warning This function relies on a single-pass XML reader.
 */
template<safety_policy safety_level>
nodes_view<safety_level> nodes(reader<safety_level>& reader)
{
	return nodes_view<safety_level>{std::make_shared<iterator_state<safety_level>>(reader)};
}

}

#endif
