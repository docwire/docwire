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

#ifndef DOCWIRE_MEMORY_BUFFER_H
#define DOCWIRE_MEMORY_BUFFER_H

#include <cstring>
#include <memory>
#include <span>

/**
  * @brief Represents a memory buffer with direct access to its data.
  * 
  * This class provides a way to work with a raw memory buffer directly.
  * 
  * It is required when direct access to the raw memory is needed without additional features
  * provided by std::vector like dynamic resizing or automatic memory management.
  * 
  * Using std::vector would introduce unnecessary overhead when only raw buffer access is required.
  * 
  * It is not straightforward to create a std::vector with uninitialized memory, and prefilling it can have a significant performance overhead due to unnecessary initialization when raw buffer access is the primary requirement.
  */
class memory_buffer
{
private:
    std::unique_ptr<std::byte[]> m_buffer;
    size_t m_size;

public:

    memory_buffer(size_t size)
#ifdef __cpp_lib_smart_ptr_for_overwrite
      : m_buffer{std::make_unique_for_overwrite<std::byte[]>(size)},
#else
      : m_buffer{std::unique_ptr<std::byte[]>(new std::byte[size])},
#endif
      m_size{size}
    {}

    std::byte* data()
    {
      return m_buffer.get();
    }

    size_t size() const
    {
        return m_size;
    }

    void resize(size_t new_size)
    {
      std::unique_ptr<std::byte[]> new_buffer = std::make_unique<std::byte[]>(new_size);
      std::memcpy(new_buffer.get(), m_buffer.get(), std::min(m_size, new_size));
      m_buffer = std::move(new_buffer);
      m_size = new_size;
    }

    std::span<std::byte> span() {
        return {m_buffer.get(), m_size};
    }
};

#endif // DOCWIRE_MEMORY_BUFFER_H
