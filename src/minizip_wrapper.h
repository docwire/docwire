/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_MINIZIP_WRAPPER_H
#define DF0_MINIZIP_WRAPPER_H

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>
#include "core_export.h"

namespace df0::detail::minizip
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

DF0_CORE_EXPORT void open(state& st, std::span<const std::byte> source);
DF0_CORE_EXPORT void close(state& st) noexcept;
DF0_CORE_EXPORT bool exists(state& st, const std::string& file_name);
DF0_CORE_EXPORT bool read(state& st, const std::string& file_name, std::string* contents, int num_of_chars);
DF0_CORE_EXPORT bool get_file_size(state& st, const std::string& file_name, unsigned long& file_size);
DF0_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, char* contents, int num_of_chars, int& readed, bool add_null_terminator);
DF0_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, std::string* contents, int num_of_chars);
DF0_CORE_EXPORT void close_reading_file_for_chunks(state& st);
DF0_CORE_EXPORT bool load_directory(state& st);

inline state::~state()
{
    close(*this);
}

} // namespace df0::detail::minizip

#endif
