/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

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

using namespace df0;

int main()
{
    df0::audit::json_stream_auditor audit{
        "*",
        df0::ref_or_owned<std::ostream>{std::clog}
    };

    std::filesystem::path{"data_processing_definition.doc"}
        | content_type::detector{}
        | office_formats_parser<
              df0::safety_policy::strict,
              df0::audit::json_stream_auditor>{
              df0::audit::auditor_ref<
                  df0::audit::json_stream_auditor>{audit}}
        | plain_text_exporter{}
        | std::cout;

    return 0;
}
