#ifndef DOCWIRE_AUDIT_DETAIL_CHAINING_HPP
#define DOCWIRE_AUDIT_DETAIL_CHAINING_HPP

#include <type_traits>

namespace docwire::audit::detail
{

struct empty {};

template<typename Next>
using next_storage_t = std::conditional_t<std::is_void_v<Next>, empty, Next>;

} // namespace docwire::audit::detail

#endif
