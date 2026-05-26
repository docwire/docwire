/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_CONTAINS_TYPE_H
#define DOCWIRE_CONTAINS_TYPE_H

#include "error.h"
#include <exception>

namespace docwire::errors
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
			if (error.context_type(i) == typeid(T))
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

} // namespace docwire::errors

#endif // DOCWIRE_CONTAINS_TYPE_H