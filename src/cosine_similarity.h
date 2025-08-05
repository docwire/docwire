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

#ifndef DOCWIRE_COSINE_SIMILARITY_H
#define DOCWIRE_COSINE_SIMILARITY_H

#include "core_export.h"
#include <vector>

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
DOCWIRE_CORE_EXPORT double cosine_similarity(const std::vector<double>& a, const std::vector<double>& b);

} // namespace docwire
#endif // DOCWIRE_COSINE_SIMILARITY_H
