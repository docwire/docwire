/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTENT_TYPE_ODF_OOXML_H
#define DF0_CONTENT_TYPE_ODF_OOXML_H

#include "pipeline/element_base.hpp"
#include "content_type_export.h"
#include "data_source.h"

namespace df0::content_type::odf_ooxml
{

/**
 * @brief Detects and assigns content types for ODF and OOXML formats.
 *
 * This detector identifies standard OpenDocument Formats (ODT, ODS, ODP, ODG) 
 * and Office Open XML formats (DOCX, XLSX, PPTX).
 * 
 * @note Performance implication: This detector prioritizes accuracy over speed. 
 * It first attempts a fast heuristic check on the beginning of the file. If that fails, 
 * it falls back to parsing the entire ZIP structure. For non-seekable streams 
 * (e.g., network sockets), this fallback may cause the entire file to be downloaded into memory.
 *
 * @param data The data source to be analyzed.
 */
DF0_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
 * @brief Detector chain element for ODF and OOXML formats.
 *
 * @see content_type::detector
 * @see content_type::odf_ooxml::detect
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

} // namespace df0::content_type::odf_ooxml

#endif // DF0_CONTENT_TYPE_ODF_OOXML_H
