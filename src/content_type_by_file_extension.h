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

#ifndef DOCWIRE_CONTENT_TYPE_BY_FILE_EXTENSION_H
#define DOCWIRE_CONTENT_TYPE_BY_FILE_EXTENSION_H

#include "chain_element.h"
#include "content_type_export.h"
#include "data_source.h"
#include "document_elements.h"

/**
 * Provides content type detection based on file extension
 */
namespace docwire::content_type::by_file_extension
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
DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
* @brief Converts a mime type to a file extension.
*
* @param mt The mime type to convert.
* @return An optional file_extension. Returns std::nullopt if no mapping is found.
*
* @see file_extension_to_mime_type
*/
DOCWIRE_CONTENT_TYPE_EXPORT std::optional<file_extension> to_extension(const mime_type& mt);

/**
* @brief Detector chain element
*
* This class represents a chain element that detects and assigns content types to data sources using file extension.
*
* @see @ref file_type_determination.cpp "performing file type detection example"
* @see content_type::detector
* @see content_type::by_file_extension::detect
*/
class detector : public ChainElement
{
public:
    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override
    {
        if (msg->is<data_source>())
        {
            data_source& data = msg->get<data_source>();
            detect(data);
        }
        else if (msg->is<document::Image>())
        {
            data_source& data = msg->get<document::Image>().source;
            detect(data);
        }
	    return emit_message(std::move(msg));
    }

    bool is_leaf() const override
	{
		return false;
	}
};

} // namespace docwire::content_type::by_file_extension

#endif // DOCWIRE_CONTENT_TYPE_BY_FILE_EXTENSION_H
