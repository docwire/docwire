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

#ifndef DOCWIRE_REF_OR_OWNED_H
#define DOCWIRE_REF_OR_OWNED_H

#include <memory>
#include <variant>

namespace docwire
{

/**
 * @brief A utility class that simplifies declaring function attributes that need to be stored without requiring the user to create a shared pointer.
 *
 * The ref_or_owned class is designed to make it easy to declare function parameters that need to store a value, without requiring the user to manually manage the memory.
 * It acts as a "sink" that can accept either a reference or a temporary object, and will automatically manage the memory for the user.
 *
 * @tparam T The type of the object being stored.
 */
template <typename T>
class ref_or_owned
{
  static_assert(!std::is_reference<T>::value, "ref_or_owned<T> cannot be instantiated with a reference type");
  static_assert(!std::is_void<T>::value, "ref_or_owned<T> cannot be instantiated with void type");

public:

  /**
   * @brief Constructs a ref_or_owned object from a reference to an object.
   *
   * The object is stored as a reference, and the ref_or_owned object will not manage its memory.
   *
   * @param value The object to be stored.
   */
  ref_or_owned(T& value)
    : v{std::reference_wrapper<T>(value)}
  {}

  /**
   * @brief Constructs a ref_or_owned object from a temporary object.
   *
   * The object is stored as a shared pointer, and the ref_or_owned object will manage its memory.
   *
   * @param value The object to be stored.
   */
  ref_or_owned(std::remove_const_t<T>&& value)
    : v{std::make_shared<T>(std::move(value))}
  {}

  /**
   * @brief Returns a const reference to the stored object, regardless of whether it is stored as a reference or a shared pointer.
   *
   * @return A const reference to the stored object.
   */
  const T& get() const {
    return get_impl();
  }

  /**
   * @brief Returns a non-const reference to the stored object, regardless of whether it is stored as a reference or a shared pointer.
   *
   * @return A non-const reference to the stored object.
   */
  T& get() {
    return get_impl();
  }

private:

  T& get_impl() const {
    if (std::holds_alternative<std::shared_ptr<T>>(v))
      return *std::get<std::shared_ptr<T>>(v);
    else
      return std::get<std::reference_wrapper<T>>(v).get();
  }

  std::variant<std::shared_ptr<T>, std::reference_wrapper<T>> v;
};

} // namespace docwire

#endif //DOCWIRE_REF_OR_OWNED_H
