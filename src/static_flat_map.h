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

#ifndef DOCWIRE_STATIC_FLAT_MAP_H
#define DOCWIRE_STATIC_FLAT_MAP_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>

namespace docwire
{

/**
 * @brief Immutable, fixed-size flat associative container.
 *
 * Stores an unsorted array of key-value pairs in the constructor and sorts them
 * in-place. Lookup is performed with binary search over contiguous memory.
 *
 * @tparam Key Key type.
 * @tparam T Mapped type.
 * @tparam N Number of entries.
 * @tparam Compare Key comparison functor.
 */
template <typename Key, typename T, std::size_t N, typename Compare = std::less<Key>>
class static_flat_map
{
public:
    /**
     * @brief Type of stored key-value pairs.
     */
    struct value_type
    {
        Key key;
        T value;
    };

    /**
     * @brief Constructs and sorts the map.
     * @param items Unsorted or sorted array of key-value pairs.
     */
    explicit static_flat_map(std::array<value_type, N> items)
        : m_items{items}
    {
        std::sort(
            m_items.begin(),
            m_items.end(),
            [](const value_type& lhs, const value_type& rhs)
            {
                return Compare{}(lhs.key, rhs.key);
            });
    }

    /**
     * @brief Finds a value by key.
     * @param key Key to search for.
     * @return Pointer to the mapped value, or nullptr when not found.
     */
    const T* find(const Key& key) const
    {
        auto it = std::lower_bound(
            m_items.begin(),
            m_items.end(),
            key,
            [](const value_type& entry, const Key& searched)
            {
                return Compare{}(entry.key, searched);
            });

        if (it == m_items.end() || Compare{}(key, it->key))
            return nullptr;

        return &it->value;
    }

    /**
     * @brief Returns the stored sorted array.
     */
    const std::array<value_type, N>& items() const noexcept
    {
        return m_items;
    }

private:
    std::array<value_type, N> m_items;
};

} // namespace docwire

#endif // DOCWIRE_STATIC_FLAT_MAP_H
