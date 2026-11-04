/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_BOOST_JSON_WRAPPER_H
#define DF0_BOOST_JSON_WRAPPER_H

#include "core_export.h"
#include "serialization_base.h"

namespace df0::serialization::detail
{
DF0_CORE_EXPORT std::string to_json_string(const value& s_val);
}

#endif // DF0_BOOST_JSON_WRAPPER_H
