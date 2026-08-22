#include "iconv_wrapper.h"
#include "gtest/gtest.h"

#include <exception>

using namespace docwire;

TEST(IconvWrapper, ConvertUtf8ToUtf16)
{
    detail::iconv::state state;
    state.from = "UTF-8";
    state.to = "UTF-16LE";

    detail::iconv::open(state);
    auto result = detail::iconv::convert(state, "hello");
    detail::iconv::close(state);

    EXPECT_FALSE(result.empty());
}

TEST(IconvWrapper, EmptyInput)
{
    detail::iconv::state state;
    state.from = "UTF-8";
    state.to = "UTF-16LE";

    detail::iconv::open(state);
    auto result = detail::iconv::convert(state, "");
    detail::iconv::close(state);

    EXPECT_TRUE(result.empty());
}

TEST(IconvWrapper, InvalidCharsetThrows)
{
    detail::iconv::state state;
    state.from = "UTF-8";
    state.to = "INVALID-CHARSET";

    EXPECT_THROW(detail::iconv::open(state), std::exception);
}
