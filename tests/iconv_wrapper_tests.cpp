#include "iconv_wrapper.h"
#include "gtest/gtest.h"

#include <exception>

using namespace docwire;

TEST(IconvWrapper, ConvertUtf8ToUtf16)
{
    detail::iconv_wrapper::state state;
    state.from = "UTF-8";
    state.to = "UTF-16LE";

    detail::iconv_wrapper::open(state);
    auto result = detail::iconv_wrapper::convert(state, "hello");
    detail::iconv_wrapper::close(state);

    EXPECT_FALSE(result.empty());
}

TEST(IconvWrapper, EmptyInput)
{
    detail::iconv_wrapper::state state;
    state.from = "UTF-8";
    state.to = "UTF-16LE";

    detail::iconv_wrapper::open(state);
    auto result = detail::iconv_wrapper::convert(state, "");
    detail::iconv_wrapper::close(state);

    EXPECT_TRUE(result.empty());
}

TEST(IconvWrapper, InvalidCharsetThrows)
{
    detail::iconv_wrapper::state state;
    state.from = "UTF-8";
    state.to = "INVALID-CHARSET";

    EXPECT_THROW(detail::iconv_wrapper::open(state), std::exception);
}
