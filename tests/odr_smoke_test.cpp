#include "type_name.h"
#include "resource_path.h"
#include "log.h"
#include "static_flat_map.h"
#include "gtest/gtest.h"

std::string odr_smoke_from_b();

TEST(ODR, Smoke)
{
    EXPECT_EQ(docwire::type_name::pretty<int>(), odr_smoke_from_b());
}
