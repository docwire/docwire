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

#ifndef DOCWIRE_MINIZIP_WRAPPER_H
#define DOCWIRE_MINIZIP_WRAPPER_H

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>
#include "core_export.h"

namespace docwire::detail::minizip
{

struct state
{
    std::span<const std::byte> source{};
    std::uintptr_t archive{}; // opaque unzFile
    std::uintptr_t buffer{};  // opaque pointer to internal zipped_buffer
    std::unordered_map<std::string, std::uintptr_t> directory;
    bool opened_for_chunks{ false };

    ~state();
};

DOCWIRE_CORE_EXPORT void open(state& st, std::span<const std::byte> source);
DOCWIRE_CORE_EXPORT void close(state& st) noexcept;
DOCWIRE_CORE_EXPORT bool exists(state& st, const std::string& file_name);
DOCWIRE_CORE_EXPORT bool read(state& st, const std::string& file_name, std::string* contents, int num_of_chars);
DOCWIRE_CORE_EXPORT bool get_file_size(state& st, const std::string& file_name, unsigned long& file_size);
DOCWIRE_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, char* contents, int num_of_chars, int& readed, bool add_null_terminator);
DOCWIRE_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, std::string* contents, int num_of_chars);
DOCWIRE_CORE_EXPORT void close_reading_file_for_chunks(state& st);
DOCWIRE_CORE_EXPORT bool load_directory(state& st);

inline state::~state()
{
    close(*this);
}

} // namespace docwire::detail::minizip

#endif
