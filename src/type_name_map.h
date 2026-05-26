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

#ifndef DOCWIRE_TYPE_NAME_MAP_H
#define DOCWIRE_TYPE_NAME_MAP_H

#include "type_name_base.h"
#include <map>
#include <unordered_map>

namespace docwire::type_name
{

template<typename Key, typename T, typename Compare, typename Alloc>
struct pretty_impl<std::map<Key, T, Compare, Alloc>> {
	std::string operator()() const { return "std::map<" + pretty<Key>() + "," + pretty<T>() + ">"; }
};

template<typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
struct pretty_impl<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> {
    std::string operator()() const { return "std::unordered_map<" + pretty<Key>() + "," + pretty<T>() + ">"; }
};

} // namespace docwire::type_name

#endif // DOCWIRE_TYPE_NAME_MAP_H
