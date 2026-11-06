/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTENT_TYPE_ASP_H
#define DF0_CONTENT_TYPE_ASP_H

#include "pipeline/element_base.hpp"
#include "content_type_export.h"
#include "data_source.h"

namespace df0::content_type::asp
{

/**
 * @brief Detects and assigns content types for ASP and ASP.NET formats.
 *
 * @param data The data source to be analyzed.
 */
DF0_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
 * @brief Detector chain element for ASP and ASP.NET formats.
 *
 * @see content_type::detector
 * @see content_type::asp::detect
 */
class detector : public pipeline::transformer_element<detector>
{
public:
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (!msg->is<data_source>())
	        return emit_message(std::move(msg));
	    data_source& data = msg->get<data_source>();
        detect(data);
        return emit_message(std::move(msg));
    }
};

} // namespace df0::content_type::asp

#endif // DF0_CONTENT_TYPE_ASP
