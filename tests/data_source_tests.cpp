#include "data_source.h"
#include "file_extension.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace docwire;

namespace
{

std::string create_datasource_test_data_str()
{
    std::vector<std::byte> test_data;
    test_data.reserve(100 * 256);
    for (int i = 0; i < 100; i++)
        for (int b = 0; b < 256; b++)
            test_data.push_back(std::byte{static_cast<unsigned char>(b)});
    std::string test_data_str = std::string(reinterpret_cast<char const*>(&test_data[0]), test_data.size()) + "test";
    return test_data_str;
}

template <typename stream_ptr_type>
void test_data_source_incremental()
{
    std::string test_data_str = create_datasource_test_data_str();
    stream_ptr_type stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr};
    for (unsigned int i = 1; i <= 9; i++)
    {
        ASSERT_EQ(data.string(length_limit{i * 256}), test_data_str.substr(0, i * 256));
        ASSERT_EQ(stream_ptr.v->tellg(), i * 256);
    }
    ASSERT_EQ(data.string(), test_data_str);
}

} // anonymous namespace

TEST(DataSource, verify_input_data)
{
    std::string test_data_str = create_datasource_test_data_str();
    ASSERT_EQ(test_data_str.size(), 100 * 256 + 4);
    ASSERT_EQ(test_data_str[0], static_cast<char>(std::byte{0}));
    ASSERT_EQ(test_data_str[255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(test_data_str[99 * 256], static_cast<char>(std::byte{0}));
    ASSERT_EQ(test_data_str[99 * 256 + 255], static_cast<char>(std::byte{255}));
    ASSERT_EQ(test_data_str[100 * 256], 't');
}

TEST(DataSource, vector_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    std::vector<std::byte> vector{reinterpret_cast<const std::byte*>(test_data_str.data()), reinterpret_cast<const std::byte*>(test_data_str.data()) + test_data_str.size()};
    data_source data{vector, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, vector_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{std::vector<std::byte>{reinterpret_cast<const std::byte*>(test_data_str.data()), reinterpret_cast<const std::byte*>(test_data_str.data()) + test_data_str.size()}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, string_view_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    std::string_view string_view{test_data_str};
    data_source data{string_view, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, string_view_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{std::string_view{test_data_str}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, seekable_stream_ptr_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    seekable_stream_ptr stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, seekable_stream_ptr_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{seekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, unseekable_stream_ptr_ref)
{
    std::string test_data_str = create_datasource_test_data_str();
    unseekable_stream_ptr stream_ptr{std::make_shared<std::istringstream>(test_data_str)};
    data_source data{stream_ptr, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, unseekable_stream_ptr_temp)
{
    std::string test_data_str = create_datasource_test_data_str();
    data_source data{unseekable_stream_ptr{std::make_shared<std::istringstream>(test_data_str)}, file_extension{".txt"}};
    ASSERT_EQ(data.string(), test_data_str);
}

TEST(DataSource, incremental_unseekable_stream_ptr)
{
    test_data_source_incremental<unseekable_stream_ptr>();
}

TEST(DataSource, incremental_seekable_stream_ptr)
{
    test_data_source_incremental<seekable_stream_ptr>();
}
