#ifndef DOCWIRE_BOOST_DEMANGLE_WRAPPER_H
#define DOCWIRE_BOOST_DEMANGLE_WRAPPER_H

#include "core_export.h"
#include <string>

namespace docwire::detail
{
DOCWIRE_CORE_EXPORT std::string demangle_type_name(const char* name);
}

#endif // DOCWIRE_BOOST_DEMANGLE_WRAPPER_H
