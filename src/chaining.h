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

#ifndef DOCWIRE_CHAINING_H
#define DOCWIRE_CHAINING_H

#include <concepts>
#include "invocation_concepts.h"
#include "invocation_traits.h"
#include "ref_or_owned.h"
#include "tuple_utils.h"

/**
 * @brief Provides functionality for chaining function calls and value transformations.
 *
 * This namespace contains classes and functions that enable the creation of chains of function calls,
 * allowing for the transformation of values in a flexible and composable way.
 */
namespace docwire::chaining
{

/**
 * @brief Binds a value to a function, enabling the creation of a function call chain.
 *
 * This class template takes two type parameters: V (the value type) and F (the function type).
 * It provides an operator() that allows the bound function to be called with the bound value
 * and additional arguments.
 *
 * @tparam V The type of the value to be bound.
 * @tparam F The type of the function to be bound.
 * @see docwire::invocation_concepts::not_invocable
 * @see docwire::invocation_concepts::invocable
 */
template<invocation_concepts::not_invocable V, invocation_concepts::invocable F>
struct value_to_function_binding
{
  /**
   * @brief The bound value.
   */
  ref_or_owned<V> value;

  /**
   * @brief The bound function.
   */
  ref_or_owned<F> function;

  /**
   * @brief Calls the bound function with the bound value and additional arguments.
   *
   * @tparam Args The types of the additional arguments.
   * @param args The additional arguments.
   * @return The result of the function call.
   */
  auto operator()(auto&&... args) const
  {
    return function.get()(value.get(), std::forward<decltype(args)>(args)...);
  }

  /**
   * @brief The type of the arguments of this functor.
   */
  using invocation_args_t = tuple_utils::remove_first_t<invocation_traits::args_t<F>>;
};

/**
 * @brief Binds a value to a function, enabling the creation of a function call chain.
 * 
 * This function template takes two type parameters: V and F (the value and function types).
 * It creates a value_to_function_binding object that allows the bound function to be called with the bound value
 * and additional arguments.
 * 
 * If the created function has no arguments, it is immediately called.
 * 
 * @param value The value to be bound.
 * @param function The function to be bound.
 * @return A value_to_function_binding object that allows the bound function to be called with the bound value
 * or invocation result if the function was immediately called..
 * @see value_to_function_binding
 * @see docwire::invocation_concepts::non_invocable
 * @see docwire::invocation_concepts::invocable
 */
template <invocation_concepts::not_invocable V, invocation_concepts::invocable F>
auto operator|(V&& value, F&& function)
{
  value_to_function_binding<std::decay_t<V>, std::decay_t<F>> binding
    { std::forward<V>(value), std::forward<F>(function) };
  if constexpr (std::tuple_size_v<invocation_traits::args_t<decltype(binding)>> == 0)
    return binding();
  else
    return binding;
}

/**
 * @brief Binds a function to a function, enabling the creation of a function call chain.
 *
 * This class template takes two type parameters: F1 and F2 (the function types).
 * It provides an operator() that allows the first function to be called with
 * specified arguments and the second function as a callback.
 * 
 * Provided arguments are divided and forwarded to the first function and the second function
 * according to their their arity.
 *
 * @tparam F1 The type of the first function to be bound.
 * @tparam F2 The type of the second function to be bound.
 * @see docwire::invocation_concepts::invocable
 */
template<invocation_concepts::invocable F1, invocation_concepts::invocable F2>
struct function_to_function_binding
{
  static_assert(invocation_traits::arity_v<F1> > 0);

  /**
   * @brief First bound function.
   */
  ref_or_owned<F1> function1;

  /**
   * @brief Second bound function.
   */
  ref_or_owned<F2> function2;

  /**
   * @brief Calls the first function with second function as a callback. Divides and forwards arguments accordingly.
   * 
   * @tparam Args The types of the arguments.
   * @param args The arguments.
   * @return The result of the function call.
   */
  auto operator()(auto&&... args) const
  {
    constexpr auto f1_arg_count = invocation_traits::arity_v<F1> - 1; // exclude callback
    constexpr auto f2_arg_count = invocation_traits::arity_v<F2> - 1; // exclude callback argument;
    static_assert(f1_arg_count + f2_arg_count == sizeof...(args));

    auto args_tuple = std::make_tuple(std::forward<decltype(args)>(args)...);

    auto f2_args = tuple_utils::subrange<f1_arg_count, f2_arg_count>(args_tuple);
    auto callback = [this, f2_args](auto&& arg) {
      auto merged_args = std::tuple_cat(std::make_tuple(std::forward<decltype(arg)>(arg)), f2_args);
      return std::apply(
        function2.get(), merged_args);
    };

    auto f1_args = tuple_utils::subrange<0, f1_arg_count>(args_tuple);
    return std::apply(function1.get(), std::tuple_cat(f1_args, std::make_tuple(callback)));
  }
  
  /**
   * @brief The type of the arguments of this functor.
   */
  using invocation_args_t =
    decltype(std::tuple_cat(
      std::declval<tuple_utils::remove_last_t<invocation_traits::args_t<F1>>>(),
      std::declval<tuple_utils::remove_first_t<invocation_traits::args_t<F2>>>()
    ));
};

/**
 * @brief Binds a function to a function, enabling the creation of a function call chain.
 * 
 * This function template takes two type parameters: F1 and F2 (the function types).
 * It creates a function_to_function_binding object that allows the first function to be called
 * with specified arguments and the second function as a callback.
 * 
 * If the created function has no arguments, it is immediately called.
 * 
 * @param function1 The first function to be bound.
 * @param function2 The second function to be bound.
 * @return A function_to_function_binding object that allows the first function to be called with
 * the second function as a callback or invocation result if the function was immediately called..
 * @see function_to_function_binding
 * @see docwire::invocation_concepts::invocable
 */
template <invocation_concepts::invocable F1, invocation_concepts::invocable F2>
auto operator|(F1&& function1, F2&& function2)
{
  function_to_function_binding<std::decay_t<F1>, std::decay_t<F2>> binding
    { std::forward<F1>(function1), std::forward<F2>(function2) };
  if constexpr (std::tuple_size_v<invocation_traits::args_t<decltype(binding)>> == 0)
    return binding();
  else
    return binding;
}

/**
 * @brief Binds a function to a pushable, enabling the creation of a function call chain.
 * 
 * This class template takes two type parameters: F (the function type) and C (the pushable type).
 * It provides an operator() that allows the bound function to be called with
 * specified arguments and the pushable push_back method as a callback.
 * 
 * @tparam F The type of the function to be bound.
 * @tparam C The type of the pushable to be bound.
 * @see docwire::invocation_concepts::invocable
 * @see docwire::invocation_concepts::pushable
 */
template<invocation_concepts::invocable F, invocation_concepts::pushable C>
struct function_to_pushable_binding
{
  /**
   * @brief The bound function.
   */
  ref_or_owned<F> function;

  /**
   * @brief The bound pushable.
   */
  ref_or_owned<C> container;

  /**
   * @brief Calls the bound function with specified arguments and the pushable push_back method as a callback.
   * 
   * @tparam Args The types of the arguments.
   * @param args The arguments.
   * @return The result of the function call.
   */
  auto operator()(auto&&... args)
  {
    using F_args_t = invocation_traits::args_t<F>;
    static_assert(std::tuple_size_v<F_args_t> > 0);
    using F_last_arg_t = tuple_utils::last_element_t<F_args_t>;
    using callback_ret_type = invocation_traits::result_t<F_last_arg_t>;
    return function.get()(std::forward<decltype(args)>(args)..., [this](auto&& arg)->callback_ret_type
    {
      container.get().push_back(arg);
      if constexpr (!std::is_void_v<callback_ret_type>)
        return callback_ret_type{};
    });
  }

  /**
   * @brief The type of the arguments of this functor.
   */
  using invocation_args_t = tuple_utils::remove_last_t<invocation_traits::args_t<F>>;
};

/**
 * @brief Binds a function to a pushable, enabling the creation of a function call chain.
 * 
 * This function template takes two type parameters: F (the function type) and C (the pushable type).
 * It creates a function_to_pushable_binding object that allows the bound function to be called with
 * specified arguments and the pushable push_back method as a callback.
 * 
 * If the created function has no arguments, it is immediately called.
 * 
 * @param function The function to be bound.
 * @param container The pushable to be bound.
 * @return A function_to_pushable_binding object that allows the bound function to be called with
 * the pushable push_back method as a callback or invocation result if the function was immediately called.
 * @see function_to_pushable_binding
 * @see docwire::invocation_concepts::invocable
 * @see docwire::invocation_concepts::pushable
 */
template <invocation_concepts::invocable F, invocation_concepts::pushable C>
auto operator|(F&& function, C&& container)
{
  function_to_pushable_binding<std::decay_t<F>, std::decay_t<C>> binding
    { std::forward<F>(function), std::forward<C>(container) };
  if constexpr (std::tuple_size_v<invocation_traits::args_t<decltype(binding)>> == 0)
    return binding();
  else
    return binding;
}

} // namespace docwire::chaining

#endif //DOCWIRE_CHAINING_H
