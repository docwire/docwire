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

#include "http_server.h"

#include "ssl_certificate.h"
#include "audit/auditor_ref.hpp"
#include "audit/scope.hpp"
#include <utility>

namespace docwire::http
{

namespace detail
{

certificate_info generate_self_signed_cert_impl(
    const std::string& common_name,
    const std::string& country,
    const std::string& organization)
{
    auto material = ::docwire::detail::ssl_certificate::generate_self_signed_pem(common_name, country, organization);
    return certificate_info{std::move(material.key), std::move(material.cert)};
}

} // namespace detail

} // namespace docwire::http
