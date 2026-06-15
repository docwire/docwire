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

#ifndef DOCWIRE_CONTENT_TYPE_HTML_H
#define DOCWIRE_CONTENT_TYPE_HTML_H

#include "chain_element.h"
#include "content_type_export.h"
#include "data_source.h"

namespace docwire::content_type::html
{

/**
 * @brief Detects and assigns content types for HTML and XHTML formats.
 *
 * @param data The data source to be analyzed.
 */
DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data);

/**
 * @brief Detector chain element for HTML and XHTML formats.
 *
 * @see content_type::detector
 * @see content_type::html::detect
 */
class detector : public chain_element
{
public:
    continuation operator()(message_ptr msg, const message_callbacks& emit_message) override
    {
        if (!msg->is<data_source>())
	        return emit_message(std::move(msg));
	    data_source& data = msg->get<data_source>();
        detect(data);
        return emit_message(std::move(msg));
    }

    bool is_leaf() const override
	{
		return false;
	}
};

} // namespace docwire::content_type::html

#endif // DOCWIRE_CONTENT_TYPE_HTML
