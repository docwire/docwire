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

#ifndef DOCWIRE_BASE64_H
#define DOCWIRE_BASE64_H

#include "base64_export.h"
#include <span>
#include <string>
#include <vector>

namespace docwire::base64
{

DOCWIRE_BASE64_EXPORT std::string encode(std::span<const std::byte> data);
DOCWIRE_BASE64_EXPORT std::vector<std::byte> decode(std::string_view data);

} // namespace docwire::base64

#endif //DOCWIRE_BASE64_H
