/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_XML_ROOT_ELEMENT_H
#define DF0_XML_ROOT_ELEMENT_H

#include "xml_children.h"
#include "xml_node_ref.h"
#include "make_error.h"
#include "error_tags.h"

namespace df0::xml
{

/**
 * @brief Finds and returns the root element of the XML document.
 * @tparam safety_level The safety policy to use.
 * @param reader The XML reader to search.
 * @throws df0::error with df0::errors::uninterpretable_data tag attached if no root element is found.
 * @warning This function advances the single-pass XML reader.
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level>
node_ref<safety_level> root_element(reader<safety_level>& reader)
{
    for (auto node: children(reader))
    {
        if (node.type() == node_type::element)
            return node;
    }
    throw DF0_MAKE_ERROR("No root element found", df0::errors::uninterpretable_data{});
}

}

#endif // DF0_XML_ROOT_ELEMENT_H
