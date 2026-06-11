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

#ifndef DOCWIRE_CONVERT_CHRONO_H
#define DOCWIRE_CONVERT_CHRONO_H

#include "convert_base.h"
#include <chrono>
#include <optional>
#include <string>
#include "with_date_format.h"

namespace docwire::convert
{

/**
 * @brief Converts an ISO 8601 date string to sys_seconds.
 */
DOCWIRE_CORE_EXPORT std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::iso8601 s, dest_type_tag<std::chrono::sys_seconds>) noexcept;
/**
 * @brief Converts a legacy OpenOffice date string to sys_seconds.
 */
DOCWIRE_CORE_EXPORT std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::openoffice_legacy s, dest_type_tag<std::chrono::sys_seconds>) noexcept;
/**
 * @brief Converts an ASN.1 date string to sys_seconds.
 */
DOCWIRE_CORE_EXPORT std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::asn1 s, dest_type_tag<std::chrono::sys_seconds>) noexcept;

// Formatting sys_seconds to string
/**
 * @brief Converts sys_seconds to a string representation.
 */
DOCWIRE_CORE_EXPORT std::optional<std::string> convert_impl(std::chrono::sys_seconds tp, dest_type_tag<std::string>) noexcept;

} // namespace docwire::convert

#endif // DOCWIRE_CONVERT_CHRONO_H
