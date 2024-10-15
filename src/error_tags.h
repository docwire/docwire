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

#ifndef DOCWIRE_ERROR_TAGS_H
#define DOCWIRE_ERROR_TAGS_H

#include <ostream>
#include <string>

namespace docwire::errors
{

/**
 * @brief Network error tag.
 *
 * This tag is used to add the information that the error chain is caused by network problem.
 * @code
 * throw make_error(errors::network_error{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::network_error> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::network_error>(e))
 *     std::cout << "Network error" << std::endl;
 * }
 * @endcode
 */
struct network_error {};

/**
 * @brief File is encrypted tag.
 *
 * This tag is used to add the information that the error chain is caused by encrypted file.
 * @code
 * throw make_error(errors::file_is_encrypted{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::file_is_encrypted> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::file_is_encrypted>(e))
 *     std::cout << "File is encrypted" << std::endl;
 * }
 * @endcode
 */
struct file_is_encrypted {};

/**
 * @brief Backtrace entry tag.
 *
 * This tag is used to add additional layer of nesting just to add information about the next part of the backtrace
 * without any additional context information
 * @code
 * catch (const errors::base& e) {
 * 	std::throw_with_nested(errors::backtrace_entry{});
 * }
 * @endcode
 */
struct backtrace_entry {};

inline network_error convert_to_context(const std::string& name, const network_error& v)
{
	return v;
}

inline std::ostream& operator<<(std::ostream& s, const network_error& e)
{
	s << "network error";
	return s;
}

inline file_is_encrypted convert_to_context(const std::string& name, const file_is_encrypted& v)
{
	return v;
}

inline std::ostream& operator<<(std::ostream& s, const file_is_encrypted& e)
{
	s << "file is encrypted";
	return s;
}

inline backtrace_entry convert_to_context(const std::string& name, const backtrace_entry& v)
{
	return v;
}

inline std::ostream& operator<<(std::ostream& s, const backtrace_entry& e)
{
	s << "backtrace entry";
	return s;
}

} // namespace docwire::errors

#endif
