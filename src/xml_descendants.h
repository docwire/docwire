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

#ifndef DOCWIRE_XML_DESCENDANTS_VIEW_H
#define DOCWIRE_XML_DESCENDANTS_VIEW_H

#include "xml_nodes.h"
#include "xml_iterator_state.h"
#include "log_scope.h"
#include "xml_node_ref.h"
#include "not_null.h"
#include <memory>
#include <ranges>

namespace docwire::xml
{

/**
 * @brief A view over all descendants of an XML node (recursive).
 *
 * @tparam safety_level The safety policy to use.
 * @warning This class relies on a single-pass XML reader. Advancing the reader invalidates this view and any iterators derived from it.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class descendants_view : public std::ranges::view_base
{
public:
	class iterator;
	iterator begin() const
	{
		return iterator{m_state, m_start_depth};
	}
	sentinel end() const { return {}; }

	/**
	 * @brief Constructs a view from an iterator state and start depth.
	 * @param state The shared iterator state.
	 * @param depth The depth of the root node of this view.
	 */
	explicit descendants_view(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state, int depth)
		: m_state(std::move(state)), m_start_depth(depth)
	{}

private:
	not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
	int m_start_depth;
};

/**
 * @brief Iterator for recursively traversing descendant nodes.
 */
template <safety_policy safety_level>
class descendants_view<safety_level>::iterator final
{
	friend class descendants_view;

public:
	using iterator_concept = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = node_ref<safety_level>;
	using pointer = const node_ref<safety_level>*;
	using reference = const node_ref<safety_level>&;

	bool operator==(const sentinel& s) const { return m_nodes_iter == s; }
	reference operator*() const { return *m_nodes_iter; }
	pointer operator->() const { return m_nodes_iter.operator->(); }
	iterator& operator++()
	{
		log::scope _{ "start_depth"_v = m_start_depth };

		if (m_state->m_node_ahead_flag)
		{
			m_state->m_node_ahead_flag = false;
		}
		else
		{
			++m_nodes_iter;
		}

		invalidate_if_out_of_scope();
		return *this;
	}
	void operator++(int)
	{
		++(*this);
	}

private:
	void invalidate_if_out_of_scope()
	{
		if (m_nodes_iter != sentinel{} && (*m_nodes_iter).depth() <= m_start_depth)
		{
			m_state->m_node_ahead_flag = true; // Mark that the next ++ should not advance the underlying reader.
			m_nodes_iter.reset(); // Invalidate this iterator to mark it as the end.
		}
	}

	explicit iterator(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state, int start_depth)
		: m_state(std::move(state)), m_start_depth(start_depth),
		  m_nodes_iter(nodes_view<safety_level>{m_state}.begin())
	{
		// If the first node is already out of scope, this iterator is invalid from the start.
		invalidate_if_out_of_scope();
	}

	not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
	int m_start_depth;
	typename nodes_view<safety_level>::iterator m_nodes_iter;
};

/**
 * @brief Returns a view of all descendants of the given node.
 * @tparam safety_level The safety policy to use.
 * @param node The node to retrieve descendants for.
 * @warning This function relies on a single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level>
descendants_view<safety_level> descendants(const node_ref<safety_level>& node)
{
	return descendants_view<safety_level>{node.state(), node.depth()};
}

/**
 * @brief Returns a view of all descendants in the reader (entire document).
 * @tparam safety_level The safety policy to use.
 * @param reader The XML reader to traverse.
 * @warning This function relies on a single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level>
descendants_view<safety_level> descendants(reader<safety_level>& reader)
{
	return descendants_view<safety_level>{assume_not_null(std::make_shared<iterator_state<safety_level>>(reader)), -1};
}

}

#endif
