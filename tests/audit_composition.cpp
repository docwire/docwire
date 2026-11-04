#include "audit.hpp"
#include "audit/filter.hpp"
#include "audit/serialize/json.hpp"
#include "audit/sink/ostream.hpp"
#include "named.h"
#include "source_location.h"

#include <iostream>

using namespace df0;

int main()
{
    auto audit =
        df0::audit::filter::by_spec{"*"}
        | df0::audit::serialize::json{}
        | df0::audit::sink::ostream{std::clog};

    audit.record(df0::source_location::current(), "audit_composition_example"_v = 42);

    return 0;
}
