/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_JSON_SERIALIZATION_H
#define DF0_JSON_SERIALIZATION_H

#include "core_export.h"
#include "serialization_base.h"
#include "boost_json_wrapper.h"

namespace df0::serialization
{

/**
 * @brief Converts a `df0::serialization::value` to a JSON string.
 *
 * This allows the generic serialization mechanism to be used for JSON-specific
 * tasks like stringification or structured logging.
 *
 * @param s_val The `df0::serialization::value` to convert.
 * @return A string containing the JSON representation of the value.
 */
inline std::string to_json(const value& s_val)
{
    return detail::to_json_string(s_val);
}

} // namespace df0::serialization

#endif // DF0_JSON_SERIALIZATION_H
