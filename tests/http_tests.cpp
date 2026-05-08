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

#include <boost/algorithm/string.hpp>
#include <boost/config.hpp>
#include <boost/json.hpp>
#include "content_type_by_file_extension.h"
#include "content_type.h"
#include "data_source.h"
#include "diagnostic_message.h"
#include <exception>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <string_view>
#include <fstream>
#include <magic_enum/magic_enum_iostream.hpp>
#include "office_formats_parser.h"
#include "output.h"
#include "plain_text_exporter.h"
#include "post.h"
#include "serialization_document_elements.h" // IWYU pragma: keep
#include "transformer_func.h"
#include "input.h"
#include "http_server.h"

using namespace docwire;

TEST(Http, Post)
{
    std::ostringstream output_stream{};
	ASSERT_NO_THROW(
	{
		std::ifstream("1.docx", std::ios_base::binary)
            | content_type::detector{}
            | office_formats_parser{}
			| PlainTextExporter()
			| http::Post("https://postman-echo.com/post")
			| output_stream;
	});

	using namespace boost::json;
	value output_val = parse(output_stream.str());
	ASSERT_TRUE(output_val.is_object());
	ASSERT_TRUE(output_val.as_object()["headers"].is_object());
	ASSERT_STREQ(output_val.as_object()["headers"].as_object()["content-type"].as_string().c_str(), "text/plain");
	ASSERT_THAT(std::string{output_val.as_object()["headers"].as_object()["user-agent"].as_string()},
                ::testing::StartsWith("DocWire SDK/"));
	ASSERT_STREQ(output_val.as_object()["data"].as_string().c_str(), "<http://www.silvercoders.com/>hyperlink test\n\n");
}

TEST(Http, PostForm)
{
    std::ostringstream output_stream{};
	ASSERT_NO_THROW(
	{
		std::ifstream("1.docx", std::ios_base::binary)
            | content_type::detector{}
            | office_formats_parser{}
            | PlainTextExporter()
            | http::Post("https://postman-echo.com/post", {{"field1", "value1"}, {"field2", "value2"}}, "file", DefaultFileName("file.docx"))
            | output_stream;
	});

    using namespace boost::json;
    value output_val = parse(output_stream.str());
    ASSERT_TRUE(output_val.is_object());
    ASSERT_TRUE(output_val.as_object()["headers"].is_object());
    ASSERT_THAT(std::string{output_val.as_object()["headers"].as_object()["content-type"].as_string()},
                ::testing::StartsWith("multipart/form-data; boundary="));
    ASSERT_STREQ(output_val.as_object()["headers"].as_object()["content-length"].as_string().c_str(), "454");
    ASSERT_TRUE(output_val.as_object()["form"].is_object());
    ASSERT_STREQ(output_val.as_object()["form"].as_object()["field1"].as_string().c_str(), "value1");
    ASSERT_STREQ(output_val.as_object()["form"].as_object()["field2"].as_string().c_str(), "value2");
    ASSERT_TRUE(output_val.as_object()["files"].is_object());
    ASSERT_STREQ(output_val.as_object()["files"].as_object()["file.txt"].as_string().c_str(), "data:application/octet-stream;base64,PGh0dHA6Ly93d3cuc2lsdmVyY29kZXJzLmNvbS8+aHlwZXJsaW5rIHRlc3QKCg==");
}

namespace {
// RAII helper to start a server in a thread and ensure it's stopped on scope exit.
struct ScopedServer {
    http::server server;
    std::thread server_thread;

    // Takes server by value to move it into the member.
    explicit ScopedServer(http::server s)
        : server(std::move(s)),
          server_thread([this]() {
              try {
                  this->server();
              } catch (const std::exception& e) {
                  // Exceptions in the server thread are test failures.
                  // The server is not expected to throw after successful startup.
                  FAIL() << "Server thread threw an unexpected exception: " << errors::diagnostic_message(e);
              }
          })
    {
        server.wait_until_ready();
    }

    ~ScopedServer() {
        server.stop();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }
};
} // namespace
class HttpServerTest : public ::testing::Test {
protected:
    const http::address addr{"127.0.0.1"};
    const std::string route_path = "/test";

    void run_server_test(const http::port& port, bool is_https)
    {
        const std::string url = (is_https ? "https://" : "http://") + addr.v + ":" + std::to_string(port.v) + route_path;

        http::server server = is_https ?
            http::server(addr, port, http::generate_self_signed_cert(addr.v, "US", "DocWire Test"), create_routes()) :
            http::server(addr, port, create_routes());

        ScopedServer server_runner{std::move(server)};
    
        std::ostringstream response_stream;
        std::string expected_response_body;
        const std::filesystem::path doc_path{"1.doc"};
        try
        {
            std::ostringstream expected_text_stream;
            data_source{doc_path} | content_type::by_file_extension::detector{} |
            office_formats_parser{} | PlainTextExporter() | expected_text_stream;
            expected_response_body = expected_text_stream.str() + " processed";
        }
        catch(const std::exception & e)
        {
            FAIL() << "Generating expected response threw an exception: " << errors::diagnostic_message(e);
        }

        try
        {
            if (is_https) {
                data_source{doc_path} | content_type::by_file_extension::detector{} |
                    http::Post(url, "", http::ssl_verify_peer{false}) | response_stream;
            } else {
                data_source{doc_path} | content_type::by_file_extension::detector{} |
                    http::Post(url) | response_stream;
            }
        }
        catch (const std::exception& e)
        {
            FAIL() << "Client pipeline threw an exception: " << errors::diagnostic_message(e);
        }
    
        EXPECT_EQ(response_stream.str(), expected_response_body);
    }

    http::server::pipeline_factory create_pipeline_factory() {
        return []() -> ParsingChain {
            return office_formats_parser{} | PlainTextExporter{} | [](message_ptr msg, const message_callbacks& emit_message) {
                if (msg->is<data_source>()) {
                    auto original_text = msg->get<data_source>().string();
                    return emit_message(data_source{original_text + " processed", mime_type{"text/plain"}, confidence::highest});
                }
                return emit_message(std::move(msg));
            };
        };
    }

    http::server::route_list create_routes() {
        http::server::route_list routes;
        routes.push_back({route_path, create_pipeline_factory()});
        return routes;
    }
};

TEST_F(HttpServerTest, ServerAndPost)
{
    run_server_test({8080}, false);
}

TEST_F(HttpServerTest, HttpsServerAndPost)
{
    try
    {
        run_server_test({8083}, true);
    }
    catch (const std::exception& e)
    {
        FAIL() << "Exception thrown: " << errors::diagnostic_message(e);
    }
}

TEST(Http, ServerErrorHandling)
{
    // Using an invalid address is a reliable way to test startup error handling
    // without race conditions from port occupation or dependency on user privileges.
    const http::address addr{"invalid-address-for-testing"};
    const http::port port{8081};

    // Create an error handler that fails the test if it's called for a startup error.
    auto test_error_handler = [](std::exception_ptr) {
        FAIL() << "Error handler should not be called for fatal startup errors.";
    };

    // Instantiate the server-under-test with the invalid address.
    http::server server_under_test(addr, port, {}, http::thread_num{1}, http::error_handler{test_error_handler});

    // Attempt to start the server. It should throw an exception because it cannot bind.
    try
    {
        server_under_test();
        FAIL() << "Expected server startup to throw an exception.";
    }
    catch(const std::exception& e)
    {
        // The diagnostic message should contain information about the bind failure.
        EXPECT_THAT(errors::diagnostic_message(e), ::testing::HasSubstr("bind"));
    }
}

TEST(Http, ServerNonFatalError)
{
    const http::address addr{"127.0.0.1"};
    const http::port port{8082}; // Use a different port to avoid conflicts

    // 1. Create an error handler that uses a promise to signal it was called.
    std::mutex error_mutex;
    std::exception_ptr error;
    auto test_error_handler = [&](std::exception_ptr eptr) {
        std::lock_guard<std::mutex> lock(error_mutex);
        error = eptr;
    };

    // 2. Create factories for an error-producing pipeline and a successful one.
    http::server::route_list routes;
    routes.push_back({"/error", []() -> ParsingChain {
        return TransformerFunc{[](message_ptr, const message_callbacks& emit_message) -> continuation {
            return emit_message(make_error_ptr("Error from pipeline processing"));
        }} | [](message_ptr msg, const message_callbacks& emit_message) {
            return emit_message(std::move(msg));
        };
    }});
    routes.push_back({"/success", []() -> ParsingChain {
        return TransformerFunc{[](message_ptr msg, const message_callbacks& emit_message) {
            return emit_message(std::move(msg));
        }} | [](message_ptr msg, const message_callbacks& emit_message)
        {
            return emit_message(std::move(msg));
        };
    }});

    // 3. Instantiate and start the server.
    ScopedServer server_runner{http::server(addr, port, std::move(routes), http::thread_num{1}, http::error_handler{test_error_handler})};

    // 4. Make a request to the error-producing endpoint. This should trigger the error handler and throw on the client side.
    ASSERT_THROW(
        {
            docwire::data_source{std::string_view{"some data"}} | http::Post("http://" + addr.v + ":" + std::to_string(port.v) + "/error") | std::ostringstream{};
        },
        std::exception
    );

    // 5. Make a request to the successful endpoint to ensure the server is still running.
    std::ostringstream success_response_stream;
    ASSERT_NO_THROW({
        docwire::data_source{std::string_view{"success data"}} | http::Post("http://" + addr.v + ":" + std::to_string(port.v) + "/success") | success_response_stream;
    });

    // 6. Verify the error handler was called and that the server is still running.
    {
        std::lock_guard<std::mutex> lock(error_mutex);
        ASSERT_THAT(errors::diagnostic_message(error), ::testing::HasSubstr("test"));
    }
    EXPECT_EQ(success_response_stream.str(), "success data");
}
