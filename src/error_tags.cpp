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

#include "error_tags.h"

namespace docwire::errors
{

std::string program_logic::string() const { return "program logic error tag"; }

std::string program_corrupted::string() const { return "program corrupted error tag"; }

std::string uninterpretable_data::string() const { return "uninterpretable data error tag"; }

std::string network_failure::string() const { return "network failure error tag"; }

std::string file_encrypted::string() const { return "file encrypted error tag"; }

} // namespace docwire::errors
