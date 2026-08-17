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

#ifndef DOCWIRE_COSINE_SIMILARITY_H
#define DOCWIRE_COSINE_SIMILARITY_H

#include "core_export.h"
#include <cmath>
#include <vector>
#include "error_tags.h"
#include "throw_if.h"

namespace docwire
{

/**
 * @brief Calculates the cosine similarity between two vectors.
 *
 * This function computes the cosine similarity between two double-precision
 * floating-point vectors. The vectors must have the same size.
 *
 * @param a The first vector.
 * @param b The second vector.
 * @return The cosine similarity, a value in the range [-1, 1].
 *         - 1: The vectors have the same orientation (maximum similarity, e.g. "a fast car" and "a quick automobile").
 *         - 0: The vectors are orthogonal (no similarity, e.g. "a fast car" and "the theory of relativity").
 *         - -1: The vectors have opposite orientations (maximum dissimilarity, e.g. "a wonderful day" and "a terrible day").
 *         Returns 0.0 if either vector has a magnitude close to zero.
 */
inline double cosine_similarity(const std::vector<double>& a, const std::vector<double>& b)
{
  DOCWIRE_THROW_IF(a.size() != b.size(), "Vectors must have the same size", errors::program_logic{});
  double dot_product = 0.0;
  double norm_a = 0.0;
  double norm_b = 0.0;
  for (size_t i = 0; i < a.size(); ++i)
  {
    dot_product += a[i] * b[i];
    norm_a += a[i] * a[i];
    norm_b += b[i] * b[i];
  }

  constexpr double zero_vector_threshold_sq = 1e-12;
  if (norm_a < zero_vector_threshold_sq || norm_b < zero_vector_threshold_sq)
    return 0.0;
  return dot_product / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

} // namespace docwire
#endif // DOCWIRE_COSINE_SIMILARITY_H
