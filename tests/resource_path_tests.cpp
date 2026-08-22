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
    auto loc = docwire::detail::program_location();
    EXPECT_FALSE(loc.empty());
}

TEST(ResourcePath, ThisLineLocationHelper)
{
    auto loc = docwire::detail::this_line_location_helper(dummy_location_function);
    EXPECT_FALSE(loc.empty());
}

TEST(ResourcePath, MissingResourceThrows)
{
    EXPECT_THROW(docwire::resource_path("__nonexistent_resource_42__"), docwire::errors::base);
}
