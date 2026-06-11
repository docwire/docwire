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

#ifndef DOCWIRE_XML_ATTRIBUTES_H
#define DOCWIRE_XML_ATTRIBUTES_H

#include "safety_policy.h"
#include "xml_node_ref.h"
#include "xml_attribute_ref.h"
#include "sentinel.h"
#include "checked.h"
#include "convert_base.h"
#include <algorithm>
#include <optional>
#include <ranges>

/**
 * @brief XML processing utilities.
 */
namespace docwire::xml
{

/**
 * @brief A view over the attributes of an XML node.
 *
 * @tparam safety_level The safety policy to use.
 * @warning This class relies on a single-pass XML reader. Advancing the reader invalidates this view and any iterators derived from it.
 * @sa xml::reader
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class attributes_view : public std::ranges::view_base
{
public:
    class iterator;
    iterator begin() const { return iterator{m_reader}; }
    /**
     * @brief Destructor.
     * Moves the reader back to the element containing the attributes to ensure consistent state for further parsing.
     */
    ~attributes_view() noexcept(noexcept(m_reader.get().move_to_element()))
    {
        m_reader.get().move_to_element();
    }
    sentinel end() const { return {}; }

    /**
     * @brief Constructs a view from a reader.
     * @param reader The XML reader positioned at the element.
     */
    explicit attributes_view(reader<safety_level>& reader) : m_reader(reader) {}
private:
    std::reference_wrapper<reader<safety_level>> m_reader;
};

/**
 * @brief A factory function to create an attributes_view with a specified safety policy.
 * @param node The XML node whose attributes will be viewed.
 * @tparam safety_level The safety policy to use.
 * @warning This function relies on a single-pass XML reader.
 * @return An attributes_view for the given node.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<safety_policy safety_level>
attributes_view<safety_level> attributes(const node_ref<safety_level>& node)
{
    return attributes_view<safety_level>{node.state()->xml_reader};
}

/**
 * @brief A convenience helper to find an attribute by name.
 * @tparam safety_level The safety policy to use.
 * @param node The XML node to search within.
 * @param name The name of the attribute to find.
 * @warning This function relies on a single-pass XML reader.
 * @return An std::optional<std::string_view> containing the attribute's value if found, otherwise std::nullopt.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<safety_policy safety_level>
checked<std::optional<std::string_view>, safety_level> attribute_value(const node_ref<safety_level>& node, std::string_view name)
{
    auto attrs = attributes<safety_level>(node);
    if (auto it = std::ranges::find_if(attrs, [name](const auto& attr) { return attr.name() == name; }); it != attrs.end())
        return (*it).value();
    return std::nullopt;
}

/**
 * @brief A convenience helper to find an attribute by name and convert its value to a specific type.
 * @tparam T The target type for conversion.
 * @tparam safety_level The safety policy for the node reference and attribute access.
 * @param node The XML node to search within.
 * @param name The name of the attribute to find.
 * @warning This function relies on a single-pass XML reader.
 * @return An std::optional<T> containing the converted value if the attribute is found and conversion succeeds, otherwise std::nullopt.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template<typename T, safety_policy safety_level>
requires convert::conversion_implementation_exists<T, std::string_view>
checked<std::optional<T>, safety_level> attribute_value(const node_ref<safety_level>& node, std::string_view name)
{
    if (auto sv = attribute_value(node, name))
        return convert::try_to<T>(*sv);
    return std::nullopt;
}

/**
 * @brief Iterator for traversing XML attributes.
 */
template<safety_policy safety_level>
class attributes_view<safety_level>::iterator
{
public:
    using iterator_concept  = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = attribute_ref<safety_level>;
    using pointer           = const value_type*;
    using reference         = const value_type&;

    bool operator==(const sentinel&) const {
        return !m_attribute.has_value();
    }

    reference operator*() const {
        return *m_attribute;
    }

    pointer operator->() const {
        return &operator*();
    }

    iterator& operator++() {
        if (m_attribute.has_value() && !m_reader.get().move_to_next_attribute()) {
            m_attribute.reset();
        }
        return *this;
    }

    void operator++(int) {
        ++(*this);
    }

private:
    friend class attributes_view<safety_level>;
    explicit iterator(std::reference_wrapper<reader<safety_level>> reader)
        : m_reader(reader) {
        if (m_reader.get().move_to_first_attribute()) {
            m_attribute.emplace(m_reader.get());
        }
    }
    std::reference_wrapper<reader<safety_level>> m_reader;
    checked<std::optional<value_type>, safety_level> m_attribute; // The presence of a value indicates a valid iterator.
};

} // namespace docwire::xml

#endif // DOCWIRE_XML_ATTRIBUTES_H
