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

#ifndef DOCWIRE_TYPE_NAME_PAIR_H
#define DOCWIRE_TYPE_NAME_PAIR_H

#include "type_name_base.h"
#include <utility>

namespace docwire::type_name
{

template<typename T1, typename T2>
struct pretty_impl<std::pair<T1, T2>> {
	std::string operator()() const { return "std::pair<" + pretty<T1>() + "," + pretty<T2>() + ">"; }
};

} // namespace docwire::type_name

#endif // DOCWIRE_TYPE_NAME_PAIR_H
