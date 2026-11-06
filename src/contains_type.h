/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTAINS_TYPE_H
#define DF0_CONTAINS_TYPE_H

#include "error.h"
#include <exception>

namespace df0::errors
{

/**
 * @brief Checks if the given nested exceptions chain contains a specific type of context.
 */
template <typename T>
bool contains_type(const std::exception& e)
{
	try
	{
		const errors::base& error = dynamic_cast<const errors::base&>(e);
		for (size_t i = 0; i < error.context_count(); ++i)
		{
			if (error.context_type_id(i) == type_id_of<T>())
				return true;
		}
	}
	catch (const std::bad_cast&)
	{
		// e is not errors::base, but may still have nested exceptions
	}

	try
	{
		std::rethrow_if_nested(e);
	}
	catch (const std::exception& nested_ex)
	{
		return contains_type<T>(nested_ex);
	}
	catch (...) {}

	return false;
}

} // namespace df0::errors

#endif // DF0_CONTAINS_TYPE_H
