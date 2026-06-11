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

#include "json_serialization.h"
#include <boost/json.hpp>

namespace docwire::serialization
{

namespace
{

boost::json::value to_json_value(const value& s_val)
{
    return std::visit(
        [](auto&& arg) -> boost::json::value {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, object>)
            {
                boost::json::object obj;
                for (const auto& [key, val] : arg.v)
                {
                    obj[key] = to_json_value(val);
                }
                return obj;
            }
            else if constexpr (std::is_same_v<T, array>)
            {
                boost::json::array arr;
                arr.reserve(arg.v.size());
                for (const auto& val : arg.v)
                {
                    arr.push_back(to_json_value(val));
                }
                return arr;
            }
            else { return boost::json::value_from(arg); }
        },
        s_val);
}

} // anonymous namespace

std::string to_json(const value& s_val)
{
    return boost::json::serialize(to_json_value(s_val));
}

} // namespace docwire::serialization
