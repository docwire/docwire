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
#include "data_source.h"
#include "export.h"

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
* @see content_type::detect
* @see content_type::detector
* @see content_type::by_file_extension::detector
*/
DOCWIRE_EXPORT void detect(data_source& data);

/**
* @brief Detector chain element
*
* This class represents a chain element that detects and assigns content types to data sources using file extension.
*
* @see content_type::detector
* @see content_type::by_file_extension::detect
*/
class detector : public ChainElement
{
public:
    void process(Info& info) override
    {
        if (!std::holds_alternative<data_source>(info.tag))
        {
	        emit(info);
		    return;
	    }
	    data_source& data = std::get<data_source>(info.tag);
        detect(data);
        emit(info);
    }

    bool is_leaf() const override
	{
		return false;
	}
};

} // namespace docwire::content_type::by_file_extension

#endif // DOCWIRE_CONTENT_TYPE_BY_FILE_EXTENSION_H
