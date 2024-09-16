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

#ifndef DOCWIRE_INVOCATION_TRAITS_H
#define DOCWIRE_INVOCATION_TRAITS_H

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>
#include <tuple>

/**
 * @brief Provides traits classes for working with callable objects.
 * 
 * This namespace contains traits classes that can be used to extract information about callable objects,
 * such as their argument types and return types. It also provides a way to check if a type is invocable.
 */
namespace docwire::invocation_traits
{

/**
 * @brief Extracts the argument types of a callable object.
 * 
 * The args template provides a convenient way to access the argument types of a callable object.
 * 
 * This template uses boost::callable_traits to extract the argument types of T, unless
 * T has a nested type invocation_args_t, in which case we use that type instead of relying on boost::callable_traits.
 * If T has a nested type invocation_args_t, this specialization uses that type instead of relying on boost::callable_traits.
 * 
 * This is necessary because boost::callable_traits doesn't support all types of callable objects,
 * like callables with auto arguments or functors with templated operator().
 * In this kind of situation functor class can provide public invocation_args_t type definiton with
 * default operator() signature for other classes to use.
 * 
 * Using this template is better than using boost::callable_traits directly because it provides a fallback mechanism
 * for cases where boost::callable_traits is not sufficient. This allows for more flexible and robust code.
 * 
 * @tparam T The type of the callable object.
 */
template <typename T>
struct args
{

  /** \privatesection */

  template <typename F>
  struct args_impl
  {
    using type = boost::callable_traits::args_t<F>;
  };

  template <typename F>
    requires requires { typename F::invocation_args_t; }
  struct args_impl<F>
  {
    using type = F::invocation_args_t;
  };

  /** \publicsection */

  /**
   * @brief Type of the arguments of the callable object.
   * 
   * This type is equal to boost::callable_traits::args_t<T> unless T has a nested type invocation_args_t,
   * in which case we use that type instead of relying on boost::callable_traits.
   */
  using type = args_impl<T>::type;
};

/**
 * @brief Type alias for the type member of the args struct.
 * 
 * The args struct is a meta-programming utility providing a convenient way to access
 * the argument types of a callable object, with a fallback mechanism
 * for cases where boost::callable_traits is not sufficient.
 * 
 * This type alias provides a shortcut for accessing the type member of the args struct.
 * 
 * @tparam T The type parameter for the args struct, typically a function type.
 * 
 * @see args
 */
template <typename T>
using args_t = typename args<T>::type;

template <typename T>
inline constexpr std::size_t arity_v = std::tuple_size_v<args_t<T>>;

/**
 * @brief Extracts the result type of a callable object.
 * 
 * The result template provides a convenient way to access the result type of a callable object.
 * 
 * This template uses boost::callable_traits to extract the result type of T, unless
 * T has a nested type invocation_result_t, in which case we use that type instead of relying on boost::callable_traits.
 * 
 * This is necessary because boost::callable_traits doesn't support all types of callable objects,
 * like callables with auto arguments or functors with templated operator().
 * In this kind of situation functor class can provide public invocation_result_t type definiton with
 * default operator() signature for other classes to use.
 * 
 * Using this template is better than using boost::callable_traits directly because it provides a fallback mechanism
 * for cases where boost::callable_traits is not sufficient. This allows for more flexible and robust code.
 * 
 * @tparam T The type of the callable object.
 */
template <typename T>
struct result
{

  /** \privatesection */

  template <typename F>
  struct result_impl
  {
    using type = boost::callable_traits::return_type_t<F>;
  };

  template <typename F>
    requires requires { typename F::invocation_result_t; }
  struct result_impl<F>
  {
    using type = F::invocation_result_t;
  };

  /** \publicsection */

  /**
     * @brief Type of the result of the callable object.
     * 
     * This type is equal to boost::callable_traits::return_type_t<T> unless T has a nested type invocation_result_t,
     * in which case we use that type instead of relying on boost::callable_traits.
     */
    using type = result_impl<T>::type;
};

/**
 * @brief Type alias for the type member of the result struct.
 * 
 * The result struct is a meta-programming utility providing a convenient way to access
 * the result type of a callable object, with a fallback mechanism
 * for cases where boost::callable_traits is not sufficient.
 * 
 * This type alias provides a shortcut for accessing the type member of the result struct.
 * 
 * @tparam T The type parameter for the result struct, typically a function type.
 * 
 * @see result
 */
template <typename T>
using result_t = typename result<T>::type;

/**
 * @brief Trait that checks if a type is invocable.
 *
 * This template provides a more flexible and convenient way to check if a type is invocable compared to `std::is_invocable`.
 * Unlike `std::is_invocable`, this trait does not require specifying the argument types, making it easier to use with types
 * that have complex or variable invocation signatures.
 *
 * Additionally, this trait supports functors with overloaded `operator()` and other types that provide a public `operator()`
 * signature through mechanisms such as `invocation_args_t`. This makes it a more comprehensive and robust solution for
 * checking invocability.
 *
 * The trait uses `boost::callable_traits` to check for invocability, and provides a fallback mechanism in cases where
 * `boost::callable_traits` is not sufficient. The fallback mechanism relies on the presence of a public `invocation_args_t`
 * type definition in the functor class.
 *
 * @tparam T The type to check for invocability.
 */
template <typename T>
struct is_invocable
{
  /**
   * @brief Whether the type is invocable.
   * 
   * This value is `true` if the type is invocable, `false` otherwise.
   */
  static constexpr bool value =
    requires { typename boost::callable_traits::args_t<T>; } ||
    requires { typename std::decay_t<T>::invocation_args_t; };
};

/**
 * @brief Variable template alias for the value of the is_invocable trait.
 *
 * This variable template provides a convenient way to access the value of the is_invocable trait as a constant expression.
 * It is equivalent to the nested value is_invocable<T>::value, but is often more readable and easier to use.
 *
 * The is_invocable trait checks if a type is invocable, meaning it can be called like a function.
 * Unlike std::is_invocable, this trait does not require specifying the argument types, making it easier to use with types
 * that have complex or variable invocation signatures.
 *
 * Additionally, this trait supports functors with overloaded `operator()` and other types that provide a public `operator()`
 * signature through mechanisms such as `invocation_args_t`. This makes it a more comprehensive and robust solution for
 * checking invocability.
 *
 * The trait uses `boost::callable_traits` to check for invocability, and provides a fallback mechanism in cases where
 * `boost::callable_traits` is not sufficient. The fallback mechanism relies on the presence of a public `invocation_args_t`
 * type definition in the functor class.
 *
 * @tparam T The type to check for invocability.
 */
template <typename T>
inline constexpr bool is_invocable_v = is_invocable<T>::value;

} // namespace docwire::invocation_traits

#endif //DOCWIRE_INVOCATION_TRAITS_H
