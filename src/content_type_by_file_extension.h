/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_CONTENT_TYPE_BY_FILE_EXTENSION_H
#define DF0_CONTENT_TYPE_BY_FILE_EXTENSION_H

#include "pipeline/element_base.hpp"
#include "content_type_export.h"
#include "data_source.h"
#include "document_elements.h"

/**
 * Provides content type detection based on file extension
 */
namespace df0::content_type::by_file_extension
{

/**
* @brief Detects and assigns content types to the provided data source using file extension.
*
* @param data The data source to be analyzed for content type detection.
*
* @see @ref file_type_determination.cpp "performing file type detection example"
* @see content_type::detect
* @see content_type::detector
* @see content_type::by_file_extension::detector
*/
DF0_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
* @brief Converts a mime type to a file extension.
*
* @param mt The mime type to convert.
* @return An optional file_extension. Returns std::nullopt if no mapping is found.
*
* @see file_extension_to_mime_type
*/
DF0_CONTENT_TYPE_EXPORT std::optional<file_extension> to_extension(const mime_type& mt);

/**
* @brief Detector chain element
*
* This class represents a chain element that detects and assigns content types to data sources using file extension.
*
* @see @ref file_type_determination.cpp "performing file type detection example"
* @see content_type::detector
* @see content_type::by_file_extension::detect
*/
class detector : public pipeline::transformer_element<detector>
{
public:
    continuation operator()(message_ptr msg, const message_callbacks& emit_message)
    {
        if (msg->is<data_source>())
        {
            data_source& data = msg->get<data_source>();
            detect(data);
        }
        else if (msg->is<document::image>())
        {
            data_source& data = msg->get<document::image>().source;
            detect(data);
        }
	    return emit_message(std::move(msg));
    }
};

} // namespace df0::content_type::by_file_extension

#endif // DF0_CONTENT_TYPE_BY_FILE_EXTENSION_H
