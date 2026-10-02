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

#include "ssl_certificate.h"

#include <cstddef>
#include <memory>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <utility>
#include "throw_if.h"

namespace docwire::detail
{

namespace
{

template <auto FreeFunction>
struct openssl_deleter
{
    template <typename T>
    void operator()(T* ptr) const noexcept { FreeFunction(ptr); }
};

template <typename T, auto FreeFunction>
using openssl_unique_ptr = std::unique_ptr<T, openssl_deleter<FreeFunction>>;

} // anonymous namespace

struct ssl_certificate::impl
{
    openssl_unique_ptr<X509, &X509_free> certificate;
    openssl_unique_ptr<EVP_PKEY, &EVP_PKEY_free> key;
};

ssl_certificate::ssl_certificate(pem material)
    : m_impl{std::make_unique<impl>()}
{
    {
        openssl_unique_ptr<BIO, &BIO_free> cert_bio{
            BIO_new_mem_buf(material.cert.data(), static_cast<int>(material.cert.size()))};
        throw_if(!cert_bio, "Failed to create certificate BIO from memory");
        m_impl->certificate.reset(PEM_read_bio_X509(cert_bio.get(), nullptr, nullptr, nullptr));
        throw_if(!m_impl->certificate, "Failed to parse certificate from memory");
    }
    {
        openssl_unique_ptr<BIO, &BIO_free> key_bio{
            BIO_new_mem_buf(material.key.data(), static_cast<int>(material.key.size()))};
        throw_if(!key_bio, "Failed to create private key BIO from memory");
        m_impl->key.reset(PEM_read_bio_PrivateKey(key_bio.get(), nullptr, nullptr, nullptr));
        throw_if(!m_impl->key, "Failed to parse private key from memory");
    }
}

ssl_certificate::ssl_certificate(ssl_certificate&& other) noexcept = default;
ssl_certificate& ssl_certificate::operator=(ssl_certificate&& other) noexcept = default;
ssl_certificate::~ssl_certificate() = default;

ssl_certificate::pem ssl_certificate::generate_self_signed_pem(
    const std::string& common_name,
    const std::string& country,
    const std::string& organization)
{
    openssl_unique_ptr<EVP_PKEY, &EVP_PKEY_free> pkey{EVP_PKEY_new()};
    throw_if(!pkey, "Failed to allocate EVP_PKEY");

    openssl_unique_ptr<RSA, &RSA_free> rsa{RSA_new()};
    throw_if(!rsa, "Failed to allocate RSA");

    openssl_unique_ptr<BIGNUM, &BN_free> bn{BN_new()};
    throw_if(!bn, "Failed to allocate BIGNUM");

    throw_if(BN_set_word(bn.get(), RSA_F4) != 1, "Failed to set RSA exponent");
    throw_if(RSA_generate_key_ex(rsa.get(), 2048, bn.get(), nullptr) != 1, "Failed to generate RSA key");
    throw_if(EVP_PKEY_assign_RSA(pkey.get(), rsa.get()) != 1, "Failed to assign RSA key to EVP_PKEY");
    rsa.release(); // ownership was transferred to pkey

    openssl_unique_ptr<X509, &X509_free> x509{X509_new()};
    throw_if(!x509, "Failed to allocate X509");

    X509_set_version(x509.get(), 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509.get()), 1);
    X509_gmtime_adj(X509_get_notBefore(x509.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(x509.get()), 31536000L); // 1 year
    throw_if(X509_set_pubkey(x509.get(), pkey.get()) != 1, "Failed to set X509 public key");

    X509_NAME* name = X509_get_subject_name(x509.get());
    throw_if(!name, "Failed to obtain X509 subject name");
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(country.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(organization.c_str()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, reinterpret_cast<const unsigned char*>(common_name.c_str()), -1, -1, 0);
    throw_if(X509_set_issuer_name(x509.get(), name) != 1, "Failed to set X509 issuer name");

    throw_if(X509_sign(x509.get(), pkey.get(), EVP_sha256()) == 0, "Failed to sign certificate");

    pem result;

    {
        openssl_unique_ptr<BIO, &BIO_free_all> key_bio{BIO_new(BIO_s_mem())};
        throw_if(!key_bio, "Failed to allocate key BIO");
        throw_if(PEM_write_bio_PrivateKey(key_bio.get(), pkey.get(), nullptr, nullptr, 0, nullptr, nullptr) != 1, "Failed to serialize private key");
        char* key_data = nullptr;
        const long key_size = BIO_get_mem_data(key_bio.get(), &key_data);
        result.key.assign(key_data, static_cast<std::size_t>(key_size));
    }

    {
        openssl_unique_ptr<BIO, &BIO_free_all> cert_bio{BIO_new(BIO_s_mem())};
        throw_if(!cert_bio, "Failed to allocate certificate BIO");
        throw_if(PEM_write_bio_X509(cert_bio.get(), x509.get()) != 1, "Failed to serialize certificate");
        char* cert_data = nullptr;
        const long cert_size = BIO_get_mem_data(cert_bio.get(), &cert_data);
        result.cert.assign(cert_data, static_cast<std::size_t>(cert_size));
    }

    return result;
}

void ssl_certificate::apply_to(void* tls_context) const
{
    auto* ssl_ctx = static_cast<SSL_CTX*>(tls_context);
    throw_if(!ssl_ctx, "Invalid TLS context");
    throw_if(SSL_CTX_use_certificate(ssl_ctx, m_impl->certificate.get()) <= 0, "Failed to apply certificate");
    throw_if(SSL_CTX_use_PrivateKey(ssl_ctx, m_impl->key.get()) <= 0, "Failed to apply private key");
}

} // namespace docwire::detail
