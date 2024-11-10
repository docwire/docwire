/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_ERROR_HASH_H
#define DOCWIRE_ERROR_HASH_H

#include <boost/container_hash/hash.hpp>
#include "error.h"

/**
 * @brief Custom hash function for docwire::errors::base objects.
 *
 * This hash function is designed to be used with unordered containers, such as
 * std::unordered_map and std::unordered_set, to store docwire::errors::base objects.
 *
 * The hash value is calculated based on the following information:
 * - File name where the error occurred
 * - Line number where the error occurred
 * - Column number where the error occurred
 * - Any nested errors of the same type (docwire::errors::base)
 *
 * @tparam docwire::errors::base The type of object being hashed.
 */
template<>
struct std::hash<docwire::errors::base>
{
	/**
     * @brief Calculates the hash value for a given docwire::errors::base object.
     *
     * @param e The docwire::errors::base object to be hashed.
     * @return size_t The calculated hash value.
     */
	size_t operator()(const docwire::errors::base& e) const noexcept
	{
		size_t hash = 0;
		boost::hash_combine(hash, std::string_view{e.source_location.file_name()});
		boost::hash_combine(hash, e.source_location.line());
		boost::hash_combine(hash, e.source_location.column());
    	try
		{
    		std::rethrow_if_nested(e);
		}
		catch (const docwire::errors::base& nested_e)
		{
			boost::hash_combine(hash, (*this)(nested_e));
		}
		catch (...)
		{
			// ignore other types of nested exceptions
    	}
    	return hash;
	}
};

#endif
