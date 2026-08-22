#ifndef DOCWIRE_BOOST_JSON_WRAPPER_H
#define DOCWIRE_BOOST_JSON_WRAPPER_H

#include "core_export.h"
#include "serialization_base.h"

namespace docwire::serialization::detail
{
DOCWIRE_CORE_EXPORT std::string to_json_string(const value& s_val);
}

#endif // DOCWIRE_BOOST_JSON_WRAPPER_H
