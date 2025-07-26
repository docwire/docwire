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

#include "cosine_similarity.h"

#include <cmath>
#include "error_tags.h"
#include "throw_if.h"

namespace docwire
{

double cosine_similarity(const std::vector<double>& a, const std::vector<double>& b)
{
  throw_if(a.size() != b.size(), "Vectors must have the same size", errors::program_logic{});
  double dot_product = 0.0;
  double norm_a = 0.0;
  double norm_b = 0.0;
  for (size_t i = 0; i < a.size(); ++i)
  {
    dot_product += a[i] * b[i];
    norm_a += a[i] * a[i];
    norm_b += b[i] * b[i];
  }

  // Use a practical epsilon for the squared norm to check for zero vectors.
  // This threshold is aligned with the one used for L2 normalization in
  // model_runner.cpp (1e-6f). The squared value is 1e-12.
  // Returning 0.0 is a common and practical approach, implying orthogonality.
  constexpr double zero_vector_threshold_sq = 1e-12;
  if (norm_a < zero_vector_threshold_sq || norm_b < zero_vector_threshold_sq)
    return 0.0;
  return dot_product / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

} // namespace docwire
