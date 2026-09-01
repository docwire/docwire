#include "error_tags.h"
#include "core_export.h"

#include <array>
#include <string_view>

namespace docwire::errors {
namespace {

using string_fn = std::string_view (*)();

} // namespace

DOCWIRE_CORE_EXPORT std::array<string_fn, 5> detail_error_tag_string_functions()
{
    return {
        &program_logic::string,
        &program_corrupted::string,
        &uninterpretable_data::string,
        &network_failure::string,
        &file_encrypted::string
    };
}

} // namespace docwire::errors
