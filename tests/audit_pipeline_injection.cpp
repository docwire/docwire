#include "audit.hpp"
#include "audit/json_stream_auditor.hpp"
#include "content_type.h"
#include "office_formats_parser.h"
#include "plain_text_exporter.h"
#include "ref_or_owned.h"
#include "pipeline/input_element.hpp"
#include "pipeline/output_element.hpp"

#include <filesystem>
#include <iostream>
#include <ostream>

using namespace docwire;

int main()
{
    docwire::audit::json_stream_auditor audit{
        "*",
        docwire::ref_or_owned<std::ostream>{std::clog}
    };

    std::filesystem::path{"data_processing_definition.doc"}
        | content_type::detector{}
        | office_formats_parser<
              docwire::safety_policy::strict,
              docwire::audit::json_stream_auditor>{
              docwire::audit::auditor_ref<
                  docwire::audit::json_stream_auditor>{audit}}
        | plain_text_exporter{}
        | std::cout;

    return 0;
}
