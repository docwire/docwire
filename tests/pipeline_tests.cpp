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

#include "pipeline/element_base.hpp"
#include "pipeline/chain_expression.hpp"
#include "pipeline/function_transformer.hpp"
#include "message.h"

#include <gtest/gtest.h>
#include <memory>
#include <type_traits>
#include <utility>

using namespace docwire;

namespace
{

struct int_source : pipeline::source_element<int_source>
{
    continuation operator()(message_ptr, const message_callbacks& emit_message)
    {
        return emit_message.further(std::make_shared<message<int>>(42));
    }
};

struct plus_one : pipeline::transformer_element<plus_one>
{
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (msg->is<int>())
            return emit_message.further(
                std::make_shared<message<int>>(msg->get<int>() + 1));
        return emit_message.further(std::move(msg));
    }
};

struct int_capture : pipeline::consumer_element<int_capture>
{
    explicit int_capture(int* output)
        : value{output}
    {
    }

    int* value = nullptr;

    continuation operator()(message_ptr msg, const message_callbacks&)
    {
        if (msg->is<int>())
            *value = msg->get<int>();
        return continuation::proceed;
    }
};

struct int_zero_source : pipeline::source_element<int_zero_source>
{
    continuation operator()(message_ptr, const message_callbacks& emit_message)
    {
        return emit_message.further(std::make_shared<message<int>>(0));
    }
};

struct back_echo_left : pipeline::transformer_element<back_echo_left>
{
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (msg->is<int>())
        {
            const int value = msg->get<int>();
            if (value == 0)
                return emit_message.further(std::string{"to_back_emitter"});
            if (value == 2)
                return emit_message.further(std::string{"from_back_echo_left"});
        }
        return emit_message.further(std::move(msg));
    }
};

struct back_emit_right : pipeline::transformer_element<back_emit_right>
{
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (msg->is<std::string>())
        {
            const std::string value = msg->get<std::string>();
            if (value == "to_back_emitter")
                return emit_message.back(2);
        }
        return emit_message.further(std::move(msg));
    }
};

struct string_capture : pipeline::consumer_element<string_capture>
{
    explicit string_capture(std::string* output)
        : output{output}
    {
    }

    std::string* output = nullptr;

    continuation operator()(message_ptr msg, const message_callbacks&)
    {
        if (msg->is<std::string>())
            *output = msg->get<std::string>();
        return continuation::proceed;
    }
};

static_assert(pipeline::source<int_zero_source>);
static_assert(pipeline::transformer<back_echo_left>);
static_assert(pipeline::transformer<back_emit_right>);
static_assert(pipeline::consumer<string_capture>);

} // namespace

static_assert(pipeline::source<int_source>);
static_assert(pipeline::transformer<plus_one>);
static_assert(pipeline::consumer<int_capture>);

static_assert(pipeline::element<int_source>);
static_assert(pipeline::element<plus_one>);
static_assert(pipeline::element<int_capture>);

static_assert(!pipeline::transformer<int_source>);
static_assert(!pipeline::consumer<plus_one>);

using source_transformer_chain =
    decltype(std::declval<int_source>() | std::declval<plus_one>());

using transformer_transformer_chain =
    decltype(std::declval<plus_one>() | std::declval<plus_one>());

using transformer_consumer_chain =
    decltype(std::declval<plus_one>() | std::declval<int_capture>());

static_assert(pipeline::chain<source_transformer_chain>);
static_assert(pipeline::chain<transformer_transformer_chain>);
static_assert(pipeline::chain<transformer_consumer_chain>);

static_assert(pipeline::source<source_transformer_chain>);
static_assert(pipeline::transformer<transformer_transformer_chain>);
static_assert(pipeline::consumer<transformer_consumer_chain>);

using complete_pipeline =
    decltype(std::declval<int_source>() | std::declval<int_capture>());

static_assert(pipeline::complete<complete_pipeline>);
static_assert(!pipeline::element<complete_pipeline>);

template <typename L, typename R>
concept can_pipe = requires(L&& lhs, R&& rhs)
{
    std::forward<L>(lhs) | std::forward<R>(rhs);
};

static_assert(!can_pipe<plus_one, int_source>);
static_assert(!can_pipe<int_capture, plus_one>);
static_assert(!can_pipe<int_capture, int_capture>);
static_assert(!can_pipe<int_source, int_source>);

TEST(pipeline, source_transformer_consumer_runtime)
{
    int result = 0;

    int_source{}
        | plus_one{}
        | int_capture{&result};

    EXPECT_EQ(result, 43);
}

TEST(pipeline, lambda_is_wrapped_as_transformer)
{
    auto passthrough = [](message_ptr msg,
                          const message_callbacks& emit_message) -> continuation
    {
        return emit_message.further(std::move(msg));
    };

    using lambda_transformer_chain =
        decltype(std::declval<plus_one>() | passthrough);

    static_assert(pipeline::transformer<lambda_transformer_chain>);

    int result = 0;

    int_source{}
        | plus_one{}
        | passthrough
        | int_capture{&result};

    EXPECT_EQ(result, 43);
}

TEST(pipeline, back_routing_from_right_to_left)
{
    std::string result;

    int_zero_source{}
        | back_echo_left{}
        | back_emit_right{}
        | string_capture{&result};

    EXPECT_EQ(result, "from_back_echo_left");
}
