#ifndef DOCWIRE_ICONV_WRAPPER_H
#define DOCWIRE_ICONV_WRAPPER_H

#include <cstdint>
#include <string>
#include <string_view>
#include "core_export.h"

namespace docwire::detail::iconv
{

struct state
{
    std::uintptr_t descriptor{}; // opaque iconv_t handle
    std::string from;
    std::string to;
};

DOCWIRE_CORE_EXPORT void open(state& st);
DOCWIRE_CORE_EXPORT void close(state& st) noexcept;
DOCWIRE_CORE_EXPORT std::string convert(state& st, std::string_view input);

} // namespace docwire::detail::iconv

#endif
