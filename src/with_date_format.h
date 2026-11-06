/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_WITH_DATE_FORMAT_H
#define DF0_WITH_DATE_FORMAT_H

#include <string_view>

/**
 * @brief Tags for specifying date formats during conversion.
 */
namespace with::date_format
{

/// ISO 8601 format (e.g., "2023-10-27T10:00:00").
struct iso8601 { std::string_view v; };

/// Legacy OpenOffice format.
struct openoffice_legacy { std::string_view v; };

/// ASN.1 format.
struct asn1 { std::string_view v; };

} // namespace with::date_format

#endif // DF0_WITH_DATE_FORMAT_H
