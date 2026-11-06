/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTENT_TYPE_OUTLOOK_H
#define DF0_CONTENT_TYPE_OUTLOOK_H

#include "pipeline/element_base.hpp"
#include "content_type_export.h"
#include "content_type_by_signature.h"
#include "data_source.h"

namespace df0::content_type::outlook
{

/**
 * @brief Detects and assigns content types for Microsoft Outlook formats.
 *
 * This detector identifies PST, OST, and MSG formats.
 *
 * @param data The data source to be analyzed.
 * @param signatures_db_to_use The loaded database of signatures used for signature-based content detection.
 */
DF0_CONTENT_TYPE_EXPORT void detect(data_source& data,
    const by_signature::database& signatures_db_to_use = by_signature::database{});

/**
 * @brief Detector chain element for Microsoft Outlook formats.
 *
 * @see content_type::detector
 * @see content_type::outlook::detect
 */
class detector : public pipeline::transformer_element<detector>
{
public:

    /**
     * @brief Constructs a new detector with the given database of signatures.
     * 
     * @param signatures_db_to_use The database of signatures to be used for content type detection.
     */
    detector(ref_or_owned<by_signature::database> signatures_db_to_use = by_signature::database{})
        : m_signatures_db_to_use(signatures_db_to_use) {}

    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (!msg->is<data_source>())
	        return emit_message(std::move(msg));
	    data_source& data = msg->get<data_source>();
        outlook::detect(data, m_signatures_db_to_use.get());
        return emit_message(std::move(msg));
    }

private:
    ref_or_owned<by_signature::database> m_signatures_db_to_use;
};

} // namespace df0::content_type::outlook

#endif // DF0_CONTENT_TYPE_OUTLOOK
