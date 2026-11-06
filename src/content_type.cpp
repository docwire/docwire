/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type.h"

#include "content_type_asp.h"
#include "content_type_by_file_extension.h"
#include "content_type_html.h"
#include "content_type_image.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_odf_ooxml.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"

namespace df0::content_type
{

void detect(data_source& data, const by_signature::database& signatures_db_to_use)
{
    content_type::by_file_extension::detect(data);
    content_type::by_signature::detect(data, signatures_db_to_use);
    content_type::image::detect(data);
    content_type::odf_ooxml::detect(data);
    content_type::asp::detect(data);
    content_type::html::detect(data);
    content_type::iwork::detect(data);
    content_type::odf_flat::detect(data);
    content_type::outlook::detect(data, signatures_db_to_use);
    content_type::xlsb::detect(data);
}

} // namespace df0::content_type
