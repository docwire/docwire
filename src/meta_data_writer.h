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

#ifndef DOCWIRE_METADATA_WRITER_H
#define DOCWIRE_METADATA_WRITER_H

#include "core_export.h"
#include <iostream>

#include "writer.h"

namespace docwire
{

class DOCWIRE_CORE_EXPORT MetaDataWriter : public Writer
{
public:
  /**
   * @brief Writes meta data of the document to an output stream.
   * @param tag data from callback
   * @param stream output stream
   */
  void write_to(const message_ptr& msg, std::ostream &stream) override;
};
} // namespace docwire

#endif //DOCWIRE_METADATA_WRITER_H
