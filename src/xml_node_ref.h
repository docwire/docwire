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

#ifndef DOCWIRE_XML_NODE_REF_H
#define DOCWIRE_XML_NODE_REF_H

#include <memory>
#include "xml_iterator_state.h"
#include "not_null.h"
#include "ranged.h"
#include "convert_base.h"

namespace docwire::xml
{

/**
 * @brief A reference to the current XML node in the reader.
 *
 * @tparam safety_level The safety policy to use.
 * @warning This reference relies on the state of a single-pass XML reader. Advancing the reader invalidates this reference.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class node_ref
{
public:
    /**
     * @brief Constructs a reference from an iterator state.
     * @param state The shared iterator state.
     */
    explicit node_ref(not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> state)
        : m_state(std::move(state)) {}
    /// Returns the local name of the node.
    std::string_view name() const { return m_state->xml_reader.name(); }
    /// Returns the full name (including namespace prefix) of the node.
    std::string_view full_name() const { return m_state->xml_reader.full_name(); }
    /// Returns the content of the node (e.g., text content).
    std::string_view content() const { return m_state->xml_reader.content(); }
    /// Returns the string value of the node (concatenated text of children).
    std::string_view string_value() const { return m_state->xml_reader.string_value(); }
    /// Returns the depth of the node in the XML tree.
    non_negative<int, safety_level> depth() const { return m_state->xml_reader.depth(); }
    /// Returns the type of the node.
    node_type type() const { return m_state->xml_reader.type(); }
    /// Returns the shared iterator state associated with this node reference.
    const not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level>& state() const { return m_state; }

private:
    not_null<std::shared_ptr<iterator_state<safety_level>>, safety_level> m_state;
};

/**
 * @brief Conversion implementation to convert a `node_ref` to another type `T`.
 * This is done by attempting to convert the node's string value.
 * @tparam T The target type.
 * @tparam safety_level The safety policy.
 */
template<typename T, safety_policy safety_level>
requires convert::conversion_implementation_exists<T, std::string_view>
std::optional<T> convert_impl(const node_ref<safety_level>& node, convert::dest_type_tag<T>) noexcept
{
    return convert::try_to<T>(node.string_value());
}

}

#endif