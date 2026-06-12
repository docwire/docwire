/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge
 * Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost
 * efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and
 * confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com */
/*  Project homepage: https://github.com/docwire/docwire */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial */

#ifndef DOCWIRE_STRONG_TYPE_H
#define DOCWIRE_STRONG_TYPE_H

#include <utility>
namespace docwire
{
template <typename T, typename Tag> class strong_type
{
  public:
    using value_type = T;

    explicit constexpr strong_type(T v) noexcept : value_(std::move(v)) {}

    constexpr T get() const noexcept { return value_; }

  private:
    T value_;
};

} // namespace docwire

#endif
