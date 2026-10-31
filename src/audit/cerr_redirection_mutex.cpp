#include "stderr_redirection.hpp"

namespace docwire::audit::detail
{

std::mutex& cerr_redirection_mutex()
{
    static std::mutex mutex;
    return mutex;
}

} // namespace docwire::audit::detail
