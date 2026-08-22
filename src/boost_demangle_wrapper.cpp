#include "boost_demangle_wrapper.h"
#include <boost/core/demangle.hpp>

namespace docwire::detail
{

std::string demangle_type_name(const char* name)
{
    return boost::core::demangle(name);
}

} // namespace docwire::detail
