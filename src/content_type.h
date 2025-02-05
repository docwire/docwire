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

#ifndef DOCWIRE_CONTENT_TYPE_H
#define DOCWIRE_CONTENT_TYPE_H

#include "chain_element.h"
#include "content_type_by_signature.h"
#include "ref_or_owned.h"

/**
 * @brief Provides content type detection related functionality
 */
namespace docwire::content_type
{

/**
 * Detects and assigns content types to the provided data source using various detection strategies.
 * 
 * This function attempts to identify the content type of the data by using the following detection methods:
 * - By file extension
 * - By file signature
 * - HTML content detection
 * - iWork content detection
 * - ODF Flat format detection
 * - Outlook format detection
 * - XLSB format detection
 * 
 * @param data The data source to be analyzed for content type detection.
 * @param signatures_db_to_use The loaded database of signatures used for signature-based content detection. It will be created (and loaded) if not provided.
 *
 * @see content_type::detector
 * @see content_type::by_signature::database
 * @see content_type::by_file_extension::detect
 * @see content_type::by_signature::detect
 * @see content_type::html::detect
 * @see content_type::iwork::detect
 * @see content_type::odf_flat::detect
 * @see content_type::outlook::detect
 * @see content_type::xlsb::detect
 */
DOCWIRE_EXPORT void detect(data_source& data, const by_signature::database& signatures_db_to_use = by_signature::database{});

/**
 * @brief Content type detection chain element
 *
 * Detects and assigns content types to the provided data source using various detection strategies.
 *
 * This class is a chain element that detects and assigns content types to data sources using the following detection methods:
 * - By file extension
 * - By file signature
 * - HTML content detection
 * - iWork content detection
 * - ODF Flat format detection
 * - Outlook format detection
 * - XLSB format detection
 *
 * @see content_type::detect
 * @see content_type::by_file_extension::detector
 * @see content_type::by_signature::detector
 * @see content_type::html::detector
 * @see content_type::iwork::detector
 * @see content_type::odf_flat::detector
 * @see content_type::outlook::detector
 * @see content_type::xlsb::detector
 */
class detector : public ChainElement
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

    void process(Info& info) override
    {
        if (!std::holds_alternative<data_source>(info.tag))
        {
	        emit(info);
		    return;
	    }
	    data_source& data = std::get<data_source>(info.tag);
        content_type::detect(data, m_signatures_db_to_use.get());
        emit(info);
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
