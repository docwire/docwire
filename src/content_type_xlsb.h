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

#ifndef DOCWIRE_CONTENT_TYPE_XLSB_H
#define DOCWIRE_CONTENT_TYPE_XLSB_H

#include "chain_element.h"
#include "data_source.h"
#include "defines.h"

namespace docwire::content_type::xlsb
{

DllExport void detect(data_source& data);

class detector : public ChainElement
{
public:
    void process(Info &info) const override
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

} // namespace docwire::content_type::xlsb

#endif // DOCWIRE_CONTENT_TYPE_XLSB
