/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "resource_path.h"
#include "gtest/gtest.h"

namespace
{
std::filesystem::path dummy_location_function()
{
    return {};
}
}

TEST(ResourcePath, ProgramLocation)
{
    auto loc = df0::detail::program_location();
    EXPECT_FALSE(loc.empty());
}

TEST(ResourcePath, ThisLineLocationHelper)
{
    auto loc = df0::detail::this_line_location_helper(dummy_location_function);
    EXPECT_FALSE(loc.empty());
}

TEST(ResourcePath, MissingResourceThrows)
{
    EXPECT_THROW(df0::resource_path("__nonexistent_resource_42__"), df0::errors::base);
}
