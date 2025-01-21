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

#ifndef DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H
#define DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H

#include "chain_element.h"
#include "data_source.h"
#include "defines.h"
#include "ref_or_owned.h"

namespace docwire::content_type::by_signature
{

struct allow_multiple
{
    bool v;
};

class database : public with_pimpl<database>
{
public:
    explicit database();
    friend DllExport void detect(data_source& data, const database& database_to_use, allow_multiple allow_multiple);
};

DllExport void detect(data_source& data, const database& database_to_use = database{}, allow_multiple allow_multiple = {false});

class detector : public ChainElement
{
public:

    explicit detector(ref_or_owned<database> database_to_use = database{}, allow_multiple allow_multiple = {false})
        : m_database_to_use(database_to_use), m_allow_multiple{allow_multiple} {}

    void process(Info& info) override
    {
        if (!std::holds_alternative<data_source>(info.tag))
        {
	        emit(info);
		    return;
	    }
	    data_source& data = std::get<data_source>(info.tag);
        detect(data, m_database_to_use.get(), m_allow_multiple);
        emit(info);
    }

    bool is_leaf() const override
	{
		return false;
	}

private:
    ref_or_owned<database> m_database_to_use;
    allow_multiple m_allow_multiple;
};

} // namespace docwire::content_type::by_signature

#endif // DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H
