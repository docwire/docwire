/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "content_type_outlook.h"

#include "content_type_by_signature.h"

namespace df0::content_type::outlook
{

void detect(data_source& data, const by_signature::database& signatures_db_to_use)
{
	if (data.highest_mime_type_confidence() >= confidence::highest)
		return;
    if (data.mime_types.empty() || data.mime_type_confidence(mime_type { "application/vnd.ms-outlook" }) >= confidence::medium)
    {
        df0::content_type::by_signature::detect(data, signatures_db_to_use, by_signature::allow_multiple{true});
        if (data.mime_type_confidence(mime_type { "application/x-ms-msg" }) < confidence::medium)
            data.add_mime_type(mime_type { "application/vnd.ms-outlook-pst" }, confidence::highest);
    }
}

} // namespace df0::content_type::outlook
