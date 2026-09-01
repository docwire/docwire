#include "type_id.h"

#include <cstdint>
#include <type_traits>

using docwire::type_id_of;

static_assert(type_id_of<int>() == type_id_of<int>());
static_assert(type_id_of<int>() != type_id_of<float>());
static_assert(type_id_of<const int>() != type_id_of<int>());
static_assert(std::is_same_v<decltype(type_id_of<int>()), docwire::type_id>);

// Known FNV-1a 64-bit vector for the empty string.
static_assert(docwire::detail::fnv1a_64("") == 14695981039346656037ull);
