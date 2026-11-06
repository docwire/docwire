/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "type_name.h"
#include "resource_path.h"
#include "static_flat_map.h"
#include "gtest/gtest.h"

std::string odr_smoke_from_b();

TEST(ODR, Smoke)
{
    EXPECT_EQ(df0::type_name::pretty<int>(), odr_smoke_from_b());
}
