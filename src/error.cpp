/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "error.h"

namespace df0::errors
{

base::base(const source_location& location)
	: location(location)
{
}

base::base(const base&) = default;
base::base(base&&) = default;
base& base::operator=(const base&) = default;
base& base::operator=(base&&) = default;
base::~base() = default;

const char* base::what() const noexcept
{
	return typeid(*this).name();
}

} // namespace df0::errors
