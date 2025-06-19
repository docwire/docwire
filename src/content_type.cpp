/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "content_type.h"

#include "content_type_asp.h"
#include "content_type_by_file_extension.h"
#include "content_type_html.h"
#include "content_type_iwork.h"
#include "content_type_odf_flat.h"
#include "content_type_outlook.h"
#include "content_type_xlsb.h"

namespace docwire::content_type
{

void detect(data_source& data, const by_signature::database& signatures_db_to_use)
{
    content_type::by_file_extension::detect(data);
    content_type::by_signature::detect(data, signatures_db_to_use);
    content_type::asp::detect(data);
    content_type::html::detect(data);
    content_type::iwork::detect(data);
    content_type::odf_flat::detect(data);
    content_type::outlook::detect(data, signatures_db_to_use);
    content_type::xlsb::detect(data);
}

} // namespace docwire::content_type
