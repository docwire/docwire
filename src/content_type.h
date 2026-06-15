/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_CONTENT_TYPE_H
#define DOCWIRE_CONTENT_TYPE_H

#include "chain_element.h"
#include "content_type_by_signature.h"
#include "ref_or_owned.h"

/**
 * @namespace docwire::content_type
 * @brief Provides a multi-stage pipeline for content type detection.
 * 
 * ### Architecture
 * The content type detection in DocWire uses a multi-stage pipeline to ensure accuracy and performance:
 * 1. **By File Extension:** Fast lookup using a comprehensive dictionary. Note that we intentionally 
 *    keep multiple MIME type aliases for a single extension (e.g., `.xml` maps to both `text/xml` 
 *    and `application/xml`) so users can query any valid historical variant.
 * 2. **By Signature (libmagic):** Reads magic bytes. 
 * 3. **Heuristic Fallbacks:** Custom detectors (e.g., HTML, OOXML, Images) that correct limitations 
 *    in signature detection.
 * 
 * ### Stream Processing & Performance
 * When processing non-seekable streams (like network sockets), signature detection cannot seek to the 
 * end of the file. This causes ZIP-based formats (DOCX, ODT) to be detected generically as `application/zip`.
 * To fix this, heuristic detectors are used. **Rule:** Heuristic detectors must prioritize performance 
 * by reading only a small initial buffer (e.g., 4KB) to check for local file headers before falling back 
 * to deep inspection (like ZIP parsing), preventing massive files from being downloaded into memory.
 */
namespace docwire::content_type
{

/**
 * Detects and assigns content types to the provided data source using various detection strategies.
 * 
 * This function attempts to identify the content type of the data by using the following detection methods:
 * - By file extension
 * - By file signature
 * - Image content detection
 * - ODF and OOXML format detection
 * - ASP content detection
 * - HTML content detection
 * - iWork content detection
 * - ODF Flat format detection
 * - Outlook format detection
 * - XLSB format detection
 * 
 * @param data The data source to be analyzed for content type detection.
 * @param signatures_db_to_use The loaded database of signatures used for signature-based content detection. It will be created (and loaded) if not provided.
 *
 * @see @ref file_type_determination.cpp "performing file type detection example"
 * @see content_type::detector
 * @see content_type::by_signature::database
 * @see content_type::by_file_extension::detect
 * @see content_type::by_signature::detect
 * @see content_type::image::detect
 * @see content_type::odf_ooxml::detect
 * @see content_type::asp::detect
 * @see content_type::html::detect
 * @see content_type::iwork::detect
 * @see content_type::odf_flat::detect
 * @see content_type::outlook::detect
 * @see content_type::xlsb::detect
 */
DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data, const by_signature::database& signatures_db_to_use = by_signature::database{});

/**
 * @brief Content type detection chain element
 *
 * Detects and assigns content types to the provided data source using various detection strategies.
 *
 * This class is a chain element that detects and assigns content types to data sources using the following detection methods:
 * - By file extension
 * - By file signature
 * - Image content detection
 * - ODF and OOXML format detection
 * - ASP content detection
 * - HTML content detection
 * - iWork content detection
 * - ODF Flat format detection
 * - Outlook format detection
 * - XLSB format detection
 *
 * @see @ref file_type_determination.cpp "performing file type detection example"
 * @see content_type::detect
 * @see content_type::by_file_extension::detector
 * @see content_type::by_signature::detector
 * @see content_type::image::detector
 * @see content_type::odf_ooxml::detector
 * @see content_type::asp::detector
 * @see content_type::html::detector
 * @see content_type::iwork::detector
 * @see content_type::odf_flat::detector
 * @see content_type::outlook::detector
 * @see content_type::xlsb::detector
 */
class detector : public chain_element
{
public:

    /**
     * @brief Constructs a new detector with the given database of signatures.
     * 
     * The detector will use the provided database of signatures for content type detection.
     * If no database is provided, it will be created and loaded.
     * 
     * @param signatures_db_to_use The database of signatures to be used for content type detection.
     * 
     * @see content_type::by_signature::database
     */
    detector(ref_or_owned<by_signature::database> signatures_db_to_use = by_signature::database{})
        : m_signatures_db_to_use(signatures_db_to_use) {}

    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override
    {
        try
        {
            if (msg->is<data_source>())
            {
                data_source& data = msg->get<data_source>();
                content_type::detect(data, m_signatures_db_to_use.get());
            }
            else if (msg->is<document::image>())
            {
                data_source& data = msg->get<document::image>().source;
                content_type::detect(data, m_signatures_db_to_use.get());
            }
        }
        catch (const std::exception& e)
        {
            emit_message(make_nested_ptr(std::current_exception(), DOCWIRE_MAKE_ERROR("Content type detection failed")));
        }
	    return emit_message(std::move(msg));
    }

    bool is_leaf() const override
	{
		return false;
	}

private:
    ref_or_owned<by_signature::database> m_signatures_db_to_use;
};

} // namespace docwire::content_type

#endif // DOCWIRE_CONTENT_TYPE
