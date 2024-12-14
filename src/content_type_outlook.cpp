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

#include "content_type_outlook.h"

#include "content_type_by_signature.h"

namespace docwire::content_type::outlook
{

void detect(data_source& data)
{
	if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    if (data.mime_types.empty() || data.mime_type_confidence(mime_type { "application/vnd.ms-outlook" }) >= confidence::medium)
    {
        docwire::content_type::by_signature::detect(data, docwire::content_type::by_signature::allow_multiple { true });
        if (data.mime_type_confidence(mime_type { "application/x-ms-msg" }) < confidence::medium)
            data.add_mime_type(mime_type { "application/vnd.ms-outlook-pst" }, confidence::highest);
    }
}

} // namespace docwire::content_type::outlook
