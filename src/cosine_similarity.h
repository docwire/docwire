/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_COSINE_SIMILARITY_H
#define DF0_COSINE_SIMILARITY_H

#include "core_export.h"
#include <cmath>
#include <vector>
#include "error_tags.h"
#include "throw_if.h"

namespace df0
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
  DF0_THROW_IF(a.size() != b.size(), "Vectors must have the same size", errors::program_logic{});
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

} // namespace df0
#endif // DF0_COSINE_SIMILARITY_H
