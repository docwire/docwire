#include "minizip_wrapper.h"
#include "gtest/gtest.h"
#include "error.h"

#include <cstddef>
#include <vector>

using namespace df0;

TEST(MinizipWrapper, OpenInvalidArchiveThrows)
{
    std::vector<std::byte> empty_zip;
    detail::minizip::state zip_state;

    EXPECT_THROW(detail::minizip::open(zip_state, empty_zip), df0::errors::base);
}
