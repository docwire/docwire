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

#ifndef DOCWIRE_LRU_MEMORY_CACHE_H
#define DOCWIRE_LRU_MEMORY_CACHE_H

#include <functional>
#include <limits>
#include <list>
#include <unordered_map>

namespace docwire
{

/**
 * @brief Least Recently Used (LRU) cache with fixed memory size.
 *
 * Cache stores key-value pairs. If cache reaches its maximum size, it evicts least recently used
 * entry from the cache.
 *
 * @tparam Key Key type
 * @tparam Value Value type
 */
template<typename Key, typename Value>
class lru_memory_cache
{
public:
    /**
     * @brief Constructs LRU cache with specified maximum size.
     * @param max_size Maximum size of the cache. Default is std::numeric_limits<size_t>::max(),
     *                 which means there is no limit.
     */
    explicit lru_memory_cache(size_t max_size = std::numeric_limits<size_t>::max())
        : m_max_size(max_size)
    {}

    /**
     * @brief Returns value for specified key. If key is not in the cache, it calls producer function
     *        to create value for the key.
     * @param key Key for which value is requested
     * @param producer Function that creates value for specified key if key is not in the cache
     * @return Reference to value for specified key
     */
    Value& get_or_create(const Key& key, const std::function<Value(const Key&)>& producer)
    {
        auto it = m_entry_list_iter_map.find(key);
        if (it != m_entry_list_iter_map.end()) {
            m_entry_list.splice(m_entry_list.begin(), m_entry_list, it->second);
        } else {
            Value value = producer(key);
            m_entry_list.emplace_front(key, value);
            m_entry_list_iter_map.emplace(key, m_entry_list.begin());
            if (m_entry_list.size() > m_max_size) {
                m_entry_list_iter_map.erase(std::prev(m_entry_list.end())->key);
                m_entry_list.pop_back();
            }
        }
        return m_entry_list.begin()->value;
    }

private:
    size_t m_max_size;
    struct entry
    {
        Key key;
        Value value;
        entry(const Key& key, const Value& value) : key(key), value(value) {}
    };
    std::list<entry> m_entry_list;
    std::unordered_map<Key, typename std::list<entry>::iterator> m_entry_list_iter_map;
};

} // namespace docwire

#endif // DOCWIRE_LRU_MEMORY_CACHE_H
