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

#ifndef DOCWIRE_XML_CHILDREN_VIEW_H
#define DOCWIRE_XML_CHILDREN_VIEW_H

#include "xml_descendants.h"
#include "log_scope.h"
#include <ranges>
#include "xml_node_ref.h"

namespace docwire::xml
{

/**
 * @brief A view over the direct children of an XML node.
 *
 * @tparam safety_level The safety policy to use.
 * @warning This class relies on a single-pass XML reader. Advancing the reader invalidates this view and any iterators derived from it.
 * @sa descendants_view
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class children_view : public std::ranges::view_base
{
public:
    class iterator;
    iterator begin() const
    {
        return iterator{m_state, m_depth};
    }
    sentinel end() const
    {
        return {};
    }

    /**
     * @brief Constructs a view from an iterator state and depth.
     * @param state The shared iterator state.
     * @param depth The target depth for children.
     */
    explicit children_view(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state, int depth)
        : m_state(std::move(state)), m_depth(depth) {}

private:
	not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
	int m_depth;
};

/**
 * @brief Iterator for traversing direct child nodes.
 */
template <safety_policy safety_level>
class children_view<safety_level>::iterator final
{
	friend class children_view<safety_level>;

public:
    using iterator_concept  = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = node_ref<safety_level>;
    using pointer           = const value_type*;
    using reference         = const value_type&;

    bool operator==(const sentinel& s) const { return m_desc_iter == s; }

    reference operator*() const { return *m_desc_iter; }
    pointer operator->() const { return m_desc_iter.operator->(); }

    iterator& operator++()
    {
        log::scope _{ "depth"_v = m_depth };
        ++m_desc_iter;
        find_next_child();
        return *this;
    }

    void operator++(int)
    {
        ++(*this);
    }

private:
    explicit iterator(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state, int depth)
        : m_depth{depth}, m_desc_iter(descendants_view<safety_level>{state, m_depth - 1}.begin())
    {
        find_next_child();
    }

    /// @brief Advances the iterator until it finds the next valid child or the end.
    void find_next_child()
    {
        while (m_desc_iter != sentinel{} && ((*m_desc_iter).depth() != m_depth || (*m_desc_iter).type() == node_type::end_element))
        {
            ++m_desc_iter;
        }
    }

    int m_depth;
    typename descendants_view<safety_level>::iterator m_desc_iter;
};

/**
 * @brief Returns a view of the direct children of the given node.
 * @tparam safety_level The safety policy to use.
 * @param node The node to retrieve children for.
 * @warning This function relies on a single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<safety_policy safety_level>
children_view<safety_level> children(const node_ref<safety_level>& node)
{
    return children_view<safety_level>{node.state(), node.depth() + 1};
}

/**
 * @brief Returns a view of the top-level children in the reader.
 * @tparam safety_level The safety policy to use.
 * @param reader The XML reader to traverse.
 * @warning This function relies on a single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<safety_policy safety_level>
children_view<safety_level> children(reader<safety_level>& reader)
{
    return children_view<safety_level>{std::make_shared<iterator_state<safety_level>>(reader), 0};
}

}

#endif