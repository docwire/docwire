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

#ifndef DOCWIRE_CONTENT_TYPE_HTML_H
#define DOCWIRE_CONTENT_TYPE_HTML_H

#include "chain_element.h"
#include "content_type_export.h"
#include "data_source.h"
#include "tags.h"

namespace docwire::content_type::html
{

DOCWIRE_CONTENT_TYPE_EXPORT void detect(data_source& data);

class detector : public ChainElement
{
public:
    continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override
    {
        if (!std::holds_alternative<data_source>(tag))
	        return emit_tag(std::move(tag));
	    data_source& data = std::get<data_source>(tag);
        detect(data);
        return emit_tag(std::move(tag));
    }

    bool is_leaf() const override
	{
		return false;
	}
};

} // namespace docwire::content_type::html

#endif // DOCWIRE_CONTENT_TYPE_HTML
