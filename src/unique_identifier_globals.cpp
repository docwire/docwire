#include "unique_identifier_globals.h"

#include <atomic>
#include <cstddef>

namespace docwire::detail
{

size_t next_unique_identifier()
{
    static std::atomic<size_t> counter{0};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

} // namespace docwire::detail
