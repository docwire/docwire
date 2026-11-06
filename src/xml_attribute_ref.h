/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_XML_ATTRIBUTE_REF_H
#define DF0_XML_ATTRIBUTE_REF_H

#include "safety_policy.h"
#include "xml_reader.h"
#include <functional>
#include <string_view>
#include <optional>
#include "convert_base.h"

namespace df0::xml
{

/**
 * @brief A reference to an XML attribute.
 * @tparam safety_level The safety policy.
 * @sa attributes_view
 * @sa @ref xml_parsing_example.cpp "XML parsing example"
 */
template <safety_policy safety_level = default_safety_level>
class attribute_ref
{
 public:
  /**
   * @brief Constructs a reference from a reader positioned at an attribute.
   * @param reader The XML reader.
   */
  explicit attribute_ref(const reader<safety_level>& reader) : m_reader(reader) {}
  /// Returns the name of the attribute.
  std::string_view name() const { return m_reader.get().name(); }
  /// Returns the value of the attribute.
  std::string_view value() const { return m_reader.get().content(); }

 private:
    std::reference_wrapper<const reader<safety_level>> m_reader;
};

/**
 * @brief Conversion implementation to convert an `attribute_ref` to another type `T`.
 * This is done by attempting to convert the attribute's string value.
 * @tparam T The target type.
 * @tparam safety_level The safety policy.
 */
template<typename T, safety_policy safety_level>
requires convert::conversion_implementation_exists<T, std::string_view>
std::optional<T> convert_impl(const attribute_ref<safety_level>& attr, convert::dest_type_tag<T>) noexcept
{
    return convert::try_to<T>(attr.value());
}

}

#endif // DF0_XML_ATTRIBUTE_REF_H