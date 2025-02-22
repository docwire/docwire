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

#ifndef DOCWIRE_WRITER_H
#define DOCWIRE_WRITER_H

#include "core_export.h"
#include "tags.h"

namespace docwire
{

class TextElement
{
public:
  TextElement() = default;
  explicit TextElement(std::string text) : text(std::move(text)) {};
  virtual void write_to(std::ostream &file) const
  {
    file << text;
  }

private:
  std::string text;
};

/**
 * @brief The Writer class is used to write data from callbacks to an output stream.
 */
class DOCWIRE_CORE_EXPORT Writer
{
public:
  /**
   * @brief writes data from callback to the specific output stream
   * @param tag data from callback
   * @param stream output stream
   */
  virtual void write_to(const Tag& tag, std::ostream &stream) = 0;
};

} // namespace docwire

#endif //DOCWIRE_WRITER_H
