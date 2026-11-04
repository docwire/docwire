/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "type_name.h"
#include "gtest/gtest.h"
#include <typeindex>
#include <string>

using namespace df0;

TEST(TypeName, FromTypeIndex)
{
    EXPECT_EQ(type_name::from_type_index(typeid(std::string)), "std::string");
}

TEST(TypeName, NormalizeClass)
{
    EXPECT_EQ(type_name::normalize_name("class std::string"), "std::string");
}

TEST(TypeName, NormalizeStruct)
{
    EXPECT_EQ(type_name::normalize_name("struct std::pair<int, std::string>"), "std::pair<int,std::string>");
}

TEST(TypeName, NormalizeStdLib)
{
    EXPECT_EQ(type_name::normalize_name("std::__1::basic_string<char>"), "std::basic_string<char>");
    EXPECT_EQ(type_name::normalize_name("std::__fs::filesystem::path"), "std::filesystem::path");
}
