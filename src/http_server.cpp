/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "http_server.h"

#include "ssl_certificate.h"
#include "audit/auditor_ref.hpp"
#include "audit/scope.hpp"
#include <utility>

namespace df0::http
{

namespace detail
{

certificate_info generate_self_signed_cert_impl(
    const std::string& common_name,
    const std::string& country,
    const std::string& organization)
{
    auto material = ::df0::detail::ssl_certificate::generate_self_signed_pem(common_name, country, organization);
    return certificate_info{std::move(material.key), std::move(material.cert)};
}

} // namespace detail

} // namespace df0::http
