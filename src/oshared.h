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

#ifndef DOCWIRE_OSHARED_H
#define DOCWIRE_OSHARED_H

#include "attributes.h"
#include <exception>
#include <functional>
#include <string>

namespace docwire
{

class Metadata;
class ThreadSafeOLEStorage;

void parse_oshared_summary_info(ThreadSafeOLEStorage& storage, attributes::Metadata& meta, const std::function<void(std::exception_ptr)>& non_fatal_error_handler);
void parse_oshared_document_summary_info(ThreadSafeOLEStorage& storage, int& slide_count);
std::string get_codepage_from_document_summary_info(ThreadSafeOLEStorage& storage);

} // namespace docwire

#endif
