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

#ifndef DOCWIRE_PLAIN_TEXT_OUTPUT_WIDTH_H
#define DOCWIRE_PLAIN_TEXT_OUTPUT_WIDTH_H

#include "../ranged.h"

namespace df0::plain_text {

/**
 * @brief Strong type representing maximum output line width for plain text.
 *
 * Values are enforced to be at least 20.
 */
class output_width {
public:
    explicit output_width(int w) : m_value(w) {}
    int value() const { return static_cast<int>(m_value); }
private:
    at_least<20, int> m_value;
};

} // namespace df0::plain_text

#endif
