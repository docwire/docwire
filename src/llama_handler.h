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

#ifndef DOCWIRE_AI_LLAMA_HANDLER_H
#define DOCWIRE_AI_LLAMA_HANDLER_H

#include <llama.h>
#include <memory>

namespace docwire::ai::llama {
/*
 * @brief A generic deleter to delete various Llama objects created on Heap memory;
 */
template <typename T> struct llama_deleter;

// handler for llama_model
template <> struct llama_deleter<llama_model> {
  void operator()(llama_model *ptr) const noexcept {
    if (ptr)
      llama_model_free(ptr);
  }
};

// handler for llama_context
template <> struct llama_deleter<llama_context> {
  void operator()(llama_context *ptr) const noexcept {
    if (ptr)
      llama_free(ptr);
  }
};

// handler for llama_sampler
template <> struct llama_deleter<llama_sampler> {
  void operator()(llama_sampler *ptr) const noexcept {
    if (ptr)
      llama_sampler_free(ptr);
  }
};

// all future handlers come below

/**
 * @brief Takes in llama model related heap initializers and attaches
 * them to their respective deleters defined above.
 *
 * Responsibilities:
 * - Passes refernces for llama heap objects to llama_deleter
 * - resets and release them as needed
 * - get() provides value as needed
 */
template <typename T> class llama_handle {
public:
  using pointer = T *;

  llama_handle() noexcept = default;

  explicit llama_handle(pointer ptr) noexcept : ptr_(ptr) {}

  ~llama_handle() = default;

  llama_handle(llama_handle &&) noexcept = default;
  llama_handle &operator=(llama_handle &&) noexcept = default;

  llama_handle(const llama_handle &) = delete;
  llama_handle &operator=(const llama_handle &) = delete;

  pointer get() const noexcept { return ptr_.get(); }

  pointer release() noexcept { return ptr_.release(); }

  void reset(pointer p = nullptr) noexcept { ptr_.reset(p); }

  explicit operator bool() const noexcept { return static_cast<bool>(ptr_); }

  pointer operator->() const noexcept { return ptr_.get(); }

  T &operator*() const noexcept { return *ptr_; }

private:
  std::unique_ptr<T, llama_deleter<T>> ptr_;
};

} // namespace docwire::ai::llama

#endif
