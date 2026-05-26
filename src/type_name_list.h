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

#ifndef DOCWIRE_TYPE_NAME_LIST_H
#define DOCWIRE_TYPE_NAME_LIST_H

#include "type_name_base.h"
#include <list>

namespace docwire::type_name
{

template<typename T, typename Alloc>
struct pretty_impl<std::list<T, Alloc>> {
	std::string operator()() const { return "std::list<" + pretty<T>() + ">"; }
};

} // namespace docwire::type_name

#endif // DOCWIRE_TYPE_NAME_LIST_H
