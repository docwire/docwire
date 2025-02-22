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

#ifndef DOCWIRE_HTML_WRITER_H
#define DOCWIRE_HTML_WRITER_H

#include "core_export.h"
#include <iostream>

#include "pimpl.h"
#include "writer.h"

namespace docwire
{

class DOCWIRE_CORE_EXPORT HtmlWriter : public Writer, public with_pimpl<HtmlWriter>
{
public:

  HtmlWriter();

  /**
   * @brief Converts text from callback to html format
   * @param tag data from callback
   * @param stream output stream
   */
  void write_to(const Tag& tag, std::ostream &stream) override;

private:
  using with_pimpl<HtmlWriter>::impl;
};
} // namespace docwire

#endif //DOCWIRE_HTML_WRITER_H
