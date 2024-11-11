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

#include <string>

namespace docwire::errors
{

/**
 * @brief Program logic failure error tag.
 *
 * This tag is used to add the information that an error occurred due to a logical inconsistency or
 * invalid assumption within the SDK itself. This is probably a mistake of an Application developer or
 * SDK developer. Should be verified and, if it looks like an error inside the SDK, reported as an issue.
 * @code
 * throw make_error(errors::program_logic{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::program_logic> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::program_logic>(e))
 *     std::cerr << "Program logic error" << std::endl;
 * }
 * @endcode
 */
struct program_logic
{
	std::string string() const { return "program logic error tag"; }
};

/**
 * @brief Program corrupted error tag.
 *
 * This tag is used to add the information that the SDK has become corrupted or is in an invalid state,
 * preventing it from functioning correctly. For example the important resource file cannot be loaded.
 * The consistency of an installation should be checked and fixed by administrator or Application developer
 * or it should be reported as an SDK issue if the reason is unknown.
 * @code
 * throw make_error(errors::program_corrupted{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::program_corrupted> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::program_corrupted>(e))
 *     std::cerr << "Program corrupted error" << std::endl;
 * }
 * @endcode
 */
struct program_corrupted
{
	std::string string() const { return "program corrupted error tag"; }
};

/**
 * @brief Uninterpretable data error tag.
 *
 * This tag is used to add the information that the SDK encountered data that it cannot interpret or understand,
 * which may be due to an incorrect file format, corrupted data, or a limitation of the SDK itself.
 * File should be checked if it's correct. If it's correct then the error can be reported as SDK bug or
 * feature request depending on details.
 * @code
 * throw make_error(errors::uninterpretable_data{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::uinterpretable_data> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::uninterpretable_data>(e))
 *     std::cerr << "Uninterpretable data error" << std::endl;
 * }
 * @endcode
 */
struct uninterpretable_data
{
	std::string string() const { return "uninterpretable data error tag"; }
};

/**
 * @brief Network failure error tag.
 *
 * This tag is used to add the information that a network-related error occurred, such as
 * a connection failure or timeout, while attempting to communicate with a remote server or resource.
 * The procedure can be restarted, remote server state should be verified and if the error reason
 * is not clear, it should be reported as an SDK issue.
 * @code
 * throw make_error(errors::network_failure{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::network_failure> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::network_failure>(e))
 *     std::cerr << "Network failure error" << std::endl;
 * }
 * @endcode
 */
struct network_failure
{
	std::string string() const { return "network failure error tag"; }
};

/**
 * @brief File is encrypted error tag.
 *
 * This tag is used to add the information that the SDK encountered a file that is encrypted
 * and cannot be accessed or processed without decryption.
 * @code
 * throw make_error(errors::file_encrypted{});
 * @endcode
 *
 * Existence of this tag can be checked using errors::contains_type<errors::file_encrypted> function.
 * @code
 * catch (const errors::base& e) {
 *   if (errors::contains_type<errors::file_encrypted>(e))
 *     std::cerr << "File encrypted error" << std::endl;
 * }
 * @endcode
 */
struct file_encrypted
{
	std::string string() const { return "file encrypted error tag"; }
};

template <typename T>
concept error_tag = std::same_as<T, program_logic> || std::same_as<T, program_corrupted> ||
	std::same_as<T, uninterpretable_data> || std::same_as<T, network_failure> || std::same_as<T, file_encrypted>;

template <error_tag T>
inline T convert_to_context(const std::string& name, const T& v)
{
	return v;
}

} // namespace docwire::errors

#endif
