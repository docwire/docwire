/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTENT_TYPE_IMAGE_H
#define DF0_CONTENT_TYPE_IMAGE_H

#include "pipeline/element_base.hpp"
#include "content_type_export.h"
#include "data_source.h"

namespace df0::content_type::image
{

/**
 * @brief Detects and assigns content types for image formats.
 *
 * This detector provides heuristic fallbacks for image formats (like BMP and WEBP) 
 * that might be misidentified by standard signature detection.
 *
 * @param data The data source to be analyzed.
 */
DF0_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
 * @brief Detector chain element for image formats.
 *
 * @see content_type::detector
 * @see content_type::image::detect
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

} // namespace df0::content_type::image

#endif // DF0_CONTENT_TYPE_IMAGE_H
