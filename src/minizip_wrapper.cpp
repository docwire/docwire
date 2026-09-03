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

#include "minizip_wrapper.h"

#include "log_scope.h"
#include "throw_if.h"
#include "unzip.h"

#include <cstring>
#include <cstdint>
#include <vector>

namespace docwire::detail::minizip
{

namespace
{

const int CASESENSITIVITY = 1;

struct zipped_buffer
{
    std::span<const std::byte> m_span;
    size_t m_pointer{};
};

voidpf buffer_open(voidpf opaque, const char* filename, int mode)
{
    (void)opaque; (void)filename; (void)mode;
    return (voidpf)1;
}

uLong buffer_read(voidpf opaque, voidpf stream, void* buf, uLong size)
{
    (void)stream;
    zipped_buffer* buffer = static_cast<zipped_buffer*>(opaque);
    size_t available = buffer->m_span.size() - buffer->m_pointer;
    size_t to_read = size < available ? size : available;
    if (to_read != 0)
    {
        std::memcpy(buf, buffer->m_span.data() + buffer->m_pointer, to_read);
        buffer->m_pointer += to_read;
    }
    return static_cast<uLong>(to_read);
}

uLong buffer_write(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    (void)opaque; (void)stream; (void)buf;
    return 0;
}

long buffer_tell(voidpf opaque, voidpf stream)
{
    (void)stream;
    return static_cast<long>(static_cast<zipped_buffer*>(opaque)->m_pointer);
}

long buffer_seek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
    (void)stream;
    zipped_buffer* buffer = static_cast<zipped_buffer*>(opaque);
    size_t position = 0;
    switch (origin)
    {
        case ZLIB_FILEFUNC_SEEK_CUR:
            position = buffer->m_pointer + offset;
            break;
        case ZLIB_FILEFUNC_SEEK_END:
            position = buffer->m_span.size() - offset;
            break;
        case ZLIB_FILEFUNC_SEEK_SET:
            position = offset;
            break;
        default:
            return -1;
    }
    if (position > buffer->m_span.size())
        position = buffer->m_span.size();
    buffer->m_pointer = position;
    return 0;
}

int buffer_close(voidpf opaque, voidpf stream)
{
    (void)opaque; (void)stream;
    return 0;
}

int buffer_error(voidpf opaque, voidpf stream)
{
    (void)opaque; (void)stream;
    return 0;
}

void clear_directory(state& st)
{
    for (auto& [name, ptr] : st.directory)
    {
        delete reinterpret_cast<unz_file_pos*>(ptr);
    }
    st.directory.clear();
}

} // anonymous namespace

DOCWIRE_CORE_EXPORT void open(state& st, std::span<const std::byte> source)
{
    st.source = source;
    auto* buffer = new zipped_buffer{source, 0};
    st.buffer = reinterpret_cast<std::uintptr_t>(buffer);

    zlib_filefunc_def read_from_buffer_functions;
    read_from_buffer_functions.zopen_file = &buffer_open;
    read_from_buffer_functions.zread_file = &buffer_read;
    read_from_buffer_functions.zwrite_file = &buffer_write;
    read_from_buffer_functions.ztell_file = &buffer_tell;
    read_from_buffer_functions.zseek_file = &buffer_seek;
    read_from_buffer_functions.zclose_file = &buffer_close;
    read_from_buffer_functions.zerror_file = &buffer_error;
    read_from_buffer_functions.opaque = buffer;

    st.archive = reinterpret_cast<std::uintptr_t>(unzOpen2("stream", &read_from_buffer_functions));
    throw_if(st.archive == 0, "Could not open zip archive");
}

DOCWIRE_CORE_EXPORT void close(state& st) noexcept
{
    if (st.archive != 0)
    {
        unzClose(reinterpret_cast<unzFile>(st.archive));
        st.archive = 0;
    }
    if (st.buffer != 0)
    {
        delete reinterpret_cast<zipped_buffer*>(st.buffer);
        st.buffer = 0;
    }
    clear_directory(st);
    st.opened_for_chunks = false;
}

DOCWIRE_CORE_EXPORT bool exists(state& st, const std::string& file_name)
{
    unzFile archive = reinterpret_cast<unzFile>(st.archive);
    return unzLocateFile(archive, file_name.c_str(), CASESENSITIVITY) == UNZ_OK;
}

DOCWIRE_CORE_EXPORT bool read(state& st, const std::string& file_name, std::string* contents, int num_of_chars)
{
    unzFile archive = reinterpret_cast<unzFile>(st.archive);
    int res;
    if (!st.directory.empty())
    {
        auto i = st.directory.find(file_name);
        if (i == st.directory.end()) return false;
        unz_file_pos* pos = reinterpret_cast<unz_file_pos*>(i->second);
        res = unzGoToFilePos(archive, pos);
    }
    else
    {
        res = unzLocateFile(archive, file_name.c_str(), CASESENSITIVITY);
    }
    if (res != UNZ_OK) return false;
    res = unzOpenCurrentFile(archive);
    if (res != UNZ_OK) return false;
    contents->clear();
    char buffer[1024 + 1];
    while ((res = unzReadCurrentFile(archive, buffer, (num_of_chars > 0 && num_of_chars < 1024) ? num_of_chars : 1024)) > 0)
    {
        buffer[res] = '\0';
        *contents += buffer;
        if (num_of_chars > 0 && contents->size() >= static_cast<size_t>(num_of_chars))
        {
            *contents = contents->substr(0, num_of_chars);
            break;
        }
    }
    if (res < 0)
    {
        unzCloseCurrentFile(archive);
        return false;
    }
    unzCloseCurrentFile(archive);
    return true;
}

DOCWIRE_CORE_EXPORT bool get_file_size(state& st, const std::string& file_name, unsigned long& file_size)
{
    unzFile archive = reinterpret_cast<unzFile>(st.archive);
    int res;
    if (!st.directory.empty())
    {
        auto i = st.directory.find(file_name);
        if (i == st.directory.end()) return false;
        unz_file_pos* pos = reinterpret_cast<unz_file_pos*>(i->second);
        res = unzGoToFilePos(archive, pos);
    }
    else
    {
        res = unzLocateFile(archive, file_name.c_str(), CASESENSITIVITY);
    }
    if (res != UNZ_OK) return false;
    unz_file_info file_info;
    if (unzGetCurrentFileInfo(archive, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
        return false;
    file_size = file_info.uncompressed_size;
    return true;
}

DOCWIRE_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, char* contents, int num_of_chars, int& readed, bool add_null_terminator)
{
    if (num_of_chars == 0)
    {
        readed = 0;
        return true;
    }

    unzFile archive = reinterpret_cast<unzFile>(st.archive);

    if (!st.opened_for_chunks)
    {
        int res;
        if (!st.directory.empty())
        {
            auto i = st.directory.find(file_name);
            if (i == st.directory.end()) return false;
            unz_file_pos* pos = reinterpret_cast<unz_file_pos*>(i->second);
            res = unzGoToFilePos(archive, pos);
        }
        else
        {
            res = unzLocateFile(archive, file_name.c_str(), CASESENSITIVITY);
        }
        if (res != UNZ_OK) return false;
        res = unzOpenCurrentFile(archive);
        if (res != UNZ_OK) return false;
        st.opened_for_chunks = true;
    }

    readed = unzReadCurrentFile(archive, contents, num_of_chars);
    if (readed < 0)
    {
        unzCloseCurrentFile(archive);
        st.opened_for_chunks = false;
        return false;
    }
    if (readed < num_of_chars)
    {
        if (add_null_terminator) contents[readed] = '\0';
        unzCloseCurrentFile(archive);
        st.opened_for_chunks = false;
        return true;
    }
    if (add_null_terminator) contents[readed] = '\0';
    return true;
}

DOCWIRE_CORE_EXPORT bool read_chunk(state& st, const std::string& file_name, std::string* contents, int num_of_chars)
{
    std::vector<char> vcontents(num_of_chars + 1);
    int readed;
    if (!read_chunk(st, file_name, vcontents.data(), num_of_chars, readed, true))
        return false;
    if (readed == 0)
        *contents = "";
    else
        contents->assign(vcontents.begin(), vcontents.begin() + readed);
    return true;
}

DOCWIRE_CORE_EXPORT void close_reading_file_for_chunks(state& st)
{
    st.opened_for_chunks = false;
}

DOCWIRE_CORE_EXPORT bool load_directory(state& st)
{
    unzFile archive = reinterpret_cast<unzFile>(st.archive);
    clear_directory(st);
    if (unzGoToFirstFile(archive) != UNZ_OK)
        return false;
    for (;;)
    {
        char name[1024];
        if (unzGetCurrentFileInfo(archive, nullptr, name, 1024, nullptr, 0, nullptr, 0) != UNZ_OK)
            return false;
        unz_file_pos* pos = new unz_file_pos;
        if (unzGetFilePos(archive, pos) != UNZ_OK)
        {
            delete pos;
            return false;
        }
        st.directory[name] = reinterpret_cast<std::uintptr_t>(pos);
        int res = unzGoToNextFile(archive);
        if (res == UNZ_END_OF_LIST_OF_FILE)
            break;
        if (res != UNZ_OK)
            return false;
    }
    return true;
}

} // namespace docwire::detail::minizip
