#include "audit.hpp"
#include "audit/filter.hpp"
#include "audit/serialize/json.hpp"
#include "audit/sink/ostream.hpp"
#include "named.h"
#include "source_location.h"

#include <iostream>

using namespace docwire;

int main()
{
    auto audit =
        docwire::audit::filter::by_spec{"*"}
        | docwire::audit::serialize::json{}
        | docwire::audit::sink::ostream{std::clog};

    audit.record(docwire::source_location::current(), "audit_composition_example"_v = 42);

    return 0;
}
