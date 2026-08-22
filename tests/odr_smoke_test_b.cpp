#include "type_name.h"
#include "resource_path.h"
#include "log.h"
#include "static_flat_map.h"
#include <string>

std::string odr_smoke_from_b()
{
    return docwire::type_name::pretty<int>();
}
