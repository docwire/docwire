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

#ifndef DOCWIRE_BOOST_JSON_WRAPPER_H
#define DOCWIRE_BOOST_JSON_WRAPPER_H

#include "core_export.h"
#include "serialization_base.h"

namespace docwire::serialization::detail
{
DOCWIRE_CORE_EXPORT std::string to_json_string(const value& s_val);
}

#endif // DOCWIRE_BOOST_JSON_WRAPPER_H
