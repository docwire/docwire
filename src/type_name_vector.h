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

#ifndef DF0_TYPE_NAME_VECTOR_H
#define DF0_TYPE_NAME_VECTOR_H

#include "type_name_base.h"
#include <string>
#include <vector>

namespace df0::type_name
{

template<typename T, typename Alloc>
struct pretty_impl<std::vector<T, Alloc>> {
	std::string operator()() const { return "std::vector<" + pretty<T>() + ">"; }
};

} // namespace df0::type_name

#endif // DF0_TYPE_NAME_VECTOR_H
