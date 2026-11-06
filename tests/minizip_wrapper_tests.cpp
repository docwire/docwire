/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

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
