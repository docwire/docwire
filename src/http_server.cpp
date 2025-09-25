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

#include "http_server.h"
#include "parsing_chain.h"
#include "exception_utils.h"
#include "data_source.h"
#include "input.h"
#include "output.h"
#include "make_error.h"
#include "throw_if.h"

#include <boost/container/flat_map.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/config.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <vector>

namespace docwire
{

namespace
{

// Helper for RAII of OpenSSL objects
template<auto F>
struct OSSLDeleter {
    template<typename T>
    void operator()(T* ptr) const { F(ptr); }
};

template<typename T, auto F>
using ossl_unique_ptr = std::unique_ptr<T, OSSLDeleter<F>>;

} // anonymous namespace

template<>
struct pimpl_impl<http::server> : pimpl_impl_base
{
	ossl_unique_ptr<X509, &X509_free> m_cert;
	ossl_unique_ptr<EVP_PKEY, &EVP_PKEY_free> m_key;
	std::unique_ptr<httplib::Server> m_svr;
    std::shared_ptr<http::error_handler_func> m_error_handler;
    size_t m_thread_num;
	std::string m_addr;
	uint16_t m_port;

    void init(http::server::route_list& routes, http::thread_num thread_num, http::body_limit limit)
    {
        if (thread_num.v > 0)
        {
            m_thread_num = thread_num.v;
            m_svr->new_task_queue = [this] { return new httplib::ThreadPool(m_thread_num); };
        }
        // Otherwise, use httplib's default thread pool.
        m_svr->set_payload_max_length(limit.v);

        m_svr->set_exception_handler([this](const httplib::Request& /*req*/, httplib::Response& res, std::exception_ptr ep) {
            (*m_error_handler)(ep);
            try {
                if (ep) {
                    std::rethrow_exception(ep);
                }
            } catch (const std::exception& e) {
                res.status = httplib::StatusCode::InternalServerError_500;
                res.set_content("Internal Server Error: " + errors::diagnostic_message(e), "text/plain");
            }
        });

        for (const auto& [path_variant, factory] : routes)
        {
            std::visit(overloaded {
                [&](const std::string& path) {
                    m_svr->Post(path, [this, factory, path](const httplib::Request& req, httplib::Response& res){
                        handle_request(req, res, factory, path);
                    });
                },
                [&](const http::regex_path& regex_p) {
                    m_svr->Post(regex_p.pattern_string.c_str(), [this, factory, path_key = regex_p.pattern_string](const httplib::Request& req, httplib::Response& res){
                        handle_request(req, res, factory, path_key);
                    });
                }
            }, path_variant);
        }
    }

	pimpl_impl(http::address addr, http::port port, http::server::route_list routes, http::thread_num thread_num, std::optional<http::certificate_info> cert_info, http::error_handler handler, http::body_limit limit)
		: m_error_handler(std::make_shared<http::error_handler_func>(std::move(handler.v))), m_addr(addr.v), m_port(port.v)
	{
		if (cert_info)
		{
            ossl_unique_ptr<BIO, &BIO_free> cert_bio(BIO_new_mem_buf(cert_info->cert.data(), (int)cert_info->cert.length()));
            throw_if(!cert_bio, "Failed to create cert BIO from memory");
            m_cert.reset(PEM_read_bio_X509(cert_bio.get(), NULL, NULL, NULL));
            throw_if(!m_cert, "Failed to parse certificate from memory");
            ossl_unique_ptr<BIO, &BIO_free> key_bio(BIO_new_mem_buf(cert_info->key.data(), (int)cert_info->key.length()));
            throw_if(!key_bio, "Failed to create key BIO from memory");
            m_key.reset(PEM_read_bio_PrivateKey(key_bio.get(), NULL, NULL, NULL));
            throw_if(!m_key, "Failed to parse private key from memory");
			m_svr = std::make_unique<httplib::SSLServer>(m_cert.get(), m_key.get());
		}
		else
		{
			m_svr = std::make_unique<httplib::Server>();
		}
        init(routes, thread_num, limit);
	}

    void handle_request(const httplib::Request& req, httplib::Response& res, const http::server::pipeline_factory& factory, const std::string& path_key)
    {
        try
        {
            thread_local boost::container::flat_map<std::string, std::unique_ptr<ParsingChain>> pipelines;
            auto& pipeline_ptr = pipelines[path_key];
            if (!pipeline_ptr)
                pipeline_ptr = std::make_unique<ParsingChain>(factory());
            ParsingChain& request_pipeline = *pipeline_ptr;

            auto response_messages = std::make_shared<std::vector<message_ptr>>();

            auto request_data_source = data_source(std::string(req.body));
            if (req.has_header("Content-Type"))
            {
                std::string content_type_header = req.get_header_value("Content-Type");
                auto semicolon_pos = content_type_header.find(';');
                std::string media_type_str = (semicolon_pos != std::string::npos)
                    ? content_type_header.substr(0, semicolon_pos)
                    : content_type_header;
                boost::algorithm::trim(media_type_str);
                if (!media_type_str.empty())
                    request_data_source.add_mime_type(mime_type{media_type_str}, confidence::high);
            }

            InputChainElement{std::move(request_data_source)} | request_pipeline | OutputChainElement{response_messages};

            if (response_messages->empty())
            {
                res.status = httplib::StatusCode::InternalServerError_500;
                res.set_content("Error: The processing pipeline did not produce any output message.", "text/plain");
                return;
            }

            message_ptr last_msg = response_messages->back();
            if (last_msg->is<data_source>())
            {
                const auto& response_data = last_msg->get<data_source>();
                res.status = httplib::StatusCode::OK_200;
                std::string content_type = "text/plain";
                if (auto mt = response_data.highest_confidence_mime_type())
                    content_type = mt->v;
                res.set_content(response_data.string(), content_type.c_str());
            }
            else if (last_msg->is<std::exception_ptr>())
            {
                (*m_error_handler)(last_msg->get<std::exception_ptr>());
                res.status = httplib::StatusCode::InternalServerError_500;
                res.set_content("Pipeline Error: " + errors::diagnostic_message(last_msg->get<std::exception_ptr>()), "text/plain");
            }
            else
            {
                res.status = httplib::StatusCode::InternalServerError_500;
                res.set_content("Error: The processing pipeline produced an unsupported message type as output.", "text/plain");
            }
        }
        catch (const std::exception& e)
        {
            (*m_error_handler)(std::current_exception());
            res.status = httplib::StatusCode::InternalServerError_500;
            res.set_content("Internal Server Error: " + errors::diagnostic_message(e), "text/plain");
        }
    }

    BOOST_NOINLINE void httplib_listen_noninlined()
    {
        m_svr->listen_after_bind();
    }

    BOOST_NOINLINE void httplib_stop_noninlined()
    {
        if (m_svr && m_svr->is_running())
        {
            m_svr->stop();
        }
    }

	~pimpl_impl()
    {
        try
        {
            stop();
        }
        catch (...)
        {
            (*m_error_handler)(std::current_exception());
        }
    }

    void run()
    {
        if (!m_svr->bind_to_port(m_addr.c_str(), m_port)) {
            throw make_error("HTTP server failed to bind", m_addr, m_port);
        }
        httplib_listen_noninlined();
    }

    void wait_until_ready() { if (m_svr) m_svr->wait_until_ready(); }

	void stop()
	{
        httplib_stop_noninlined();
	}
};

namespace http
{

server::server(address addr, port port, route_list routes, thread_num thread_num, error_handler handler, body_limit limit)
	: with_pimpl<server>(addr, port, std::move(routes), thread_num, std::nullopt, std::move(handler), limit)
{}

server::server(address addr, port port, certificate_info cert_info, route_list routes, thread_num thread_num, error_handler handler, body_limit limit)
	: with_pimpl<server>(addr, port, std::move(routes), thread_num, std::move(cert_info), std::move(handler), limit)
{}

server::~server() = default;

server::server(server&&) = default;

server& server::operator=(server&&) = default;

void server::operator()()
{
    try
    {
        impl().run();
    }
    catch(const std::exception& e)
    {
		std::throw_with_nested(make_error("HTTP server failed to start"));
    }
}

void server::wait_until_ready()
{
    impl().wait_until_ready();
}

void server::stop()
{
	impl().stop();
}

http::certificate_info generate_self_signed_cert(const std::string& common_name, const std::string& country, const std::string& organization)
{
    // 1. Create key
    ossl_unique_ptr<EVP_PKEY, &EVP_PKEY_free> pkey(EVP_PKEY_new());
    ossl_unique_ptr<RSA, &RSA_free> rsa(RSA_new());
    ossl_unique_ptr<BIGNUM, &BN_free> bn(BN_new());
    BN_set_word(bn.get(), RSA_F4);
    RSA_generate_key_ex(rsa.get(), 2048, bn.get(), NULL);
    EVP_PKEY_assign_RSA(pkey.get(), rsa.release());

    // 2. Create X509 certificate
    ossl_unique_ptr<X509, &X509_free> x509(X509_new());
    X509_set_version(x509.get(), 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509.get()), 1);
    X509_gmtime_adj(X509_get_notBefore(x509.get()), 0);
    X509_gmtime_adj(X509_get_notAfter(x509.get()), 31536000L); // 1 year
    X509_set_pubkey(x509.get(), pkey.get());

    // 3. Set subject and issuer name
    X509_NAME *name = X509_get_subject_name(x509.get());
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char *)country.c_str(), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (const unsigned char *)organization.c_str(), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char *)common_name.c_str(), -1, -1, 0);
    X509_set_issuer_name(x509.get(), name);

    // 4. Sign certificate
    throw_if(X509_sign(x509.get(), pkey.get(), EVP_sha256()) == 0, "Failed to sign certificate");

    // 5. Convert to PEM strings
    ossl_unique_ptr<BIO, &BIO_free_all> key_bio(BIO_new(BIO_s_mem()));
    PEM_write_bio_PrivateKey(key_bio.get(), pkey.get(), NULL, NULL, 0, NULL, NULL);
    char *key_data;
    long key_len = BIO_get_mem_data(key_bio.get(), &key_data);
    std::string key_str(key_data, key_len);

    ossl_unique_ptr<BIO, &BIO_free_all> cert_bio(BIO_new(BIO_s_mem()));
    PEM_write_bio_X509(cert_bio.get(), x509.get());
    char *cert_data;
    long cert_len = BIO_get_mem_data(cert_bio.get(), &cert_data);
    std::string cert_str(cert_data, cert_len);

    return {key_str, cert_str};
}

} // namespace http
} // namespace docwire
