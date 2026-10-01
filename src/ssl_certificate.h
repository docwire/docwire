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

#ifndef DOCWIRE_DETAIL_SSL_CERTIFICATE_H
#define DOCWIRE_DETAIL_SSL_CERTIFICATE_H

#include <memory>
#include <string>

namespace docwire::detail
{

/**
 * @brief A thin wrapper around an opaque TLS private key / certificate pair.
 *
 * This wrapper owns the third-party TLS objects (X509, EVP_PKEY, BIO) and
 * hides them from the rest of the SDK so that no third-party header leaks
 * into the public boundary. Its implementation is quarantined inside a
 * dedicated translation unit.
 */
class ssl_certificate
{
public:
    /// PEM-encoded certificate/key material.
    struct pem
    {
        std::string key;
        std::string cert;
    };

    /**
     * @brief Constructs the wrapper from PEM-encoded material.
     *
     * @throws std::exception If the given material cannot be parsed.
     */
    explicit ssl_certificate(pem material);

    ~ssl_certificate();

    ssl_certificate(ssl_certificate&& other) noexcept;
    ssl_certificate& operator=(ssl_certificate&& other) noexcept;

    ssl_certificate(const ssl_certificate&) = delete;
    ssl_certificate& operator=(const ssl_certificate&) = delete;

    /**
     * @brief Generates a new self-signed certificate and private key.
     *
     * @param common_name The common name (CN) for the certificate.
     * @param country The country name (C) for the certificate.
     * @param organization The organization name (O) for the certificate.
     */
    static pem generate_self_signed_pem(
        const std::string& common_name,
        const std::string& country,
        const std::string& organization);

    /**
     * @brief Applies the certificate and key to an opaque TLS context.
     *
     * @param tls_context Pointer to the underlying third-party TLS context.
     *                    Passed as `void*` so the third-party type does not leak
     *                    into this header.
     */
    void apply_to(void* tls_context) const;

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};

} // namespace docwire::detail

#endif // DOCWIRE_DETAIL_SSL_CERTIFICATE_H
