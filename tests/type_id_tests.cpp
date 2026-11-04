#include "type_id.h"

#include <cstdint>
#include <type_traits>

using df0::type_id_of;

static_assert(type_id_of<int>() == type_id_of<int>());
static_assert(type_id_of<int>() != type_id_of<float>());
static_assert(type_id_of<const int>() != type_id_of<int>());
static_assert(std::is_same_v<decltype(type_id_of<int>()), df0::type_id>);

// Known FNV-1a 64-bit vector for the empty string.
static_assert(df0::detail::fnv1a_64("") == 14695981039346656037ull);
