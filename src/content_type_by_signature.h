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

#include "data_source.h"
#include "defines.h"

namespace docwire::content_type::by_signature
{

struct allow_multiple
{
    bool v;
};

DllExport void detect(data_source& data, allow_multiple allow_multiple = {false});

} // namespace docwire::content_type::by_signature

#endif // DOCWIRE_CONTENT_TYPE_BY_SIGNATURE_H
