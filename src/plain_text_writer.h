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

#ifndef DOCWIRE_PLAIN_TEXT_WRITER_H
#define DOCWIRE_PLAIN_TEXT_WRITER_H

#include "core_export.h"
#include <iostream>

#include "pimpl.h"
#include "writer.h"
#include "message.h"
#include "document_elements.h"

namespace docwire
{

class DOCWIRE_CORE_EXPORT PlainTextWriter : public Writer, public with_pimpl<PlainTextWriter>
{
public:
  PlainTextWriter(const std::string& eol_sequence,
    std::function<std::string(const document::Link&)> format_link_opening,
    std::function<std::string(const document::CloseLink&)> format_link_closing);

  /**
   * @brief Converts text from callback to plain text format.
   * @param msg data from callback
   * @param stream output stream
   */
  void write_to(const message_ptr& msg, std::ostream &stream) override;

  const std::string eol_sequence() const;
};
} // namespace docwire

#endif //DOCWIRE_PLAIN_TEXT_WRITER_H
