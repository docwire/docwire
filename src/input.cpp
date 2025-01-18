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

#include "input.h"

#include "log.h"
#include "log_data_source.h" // IWYU pragma: keep

using namespace docwire;

void InputChainElement::process(Info &info)
{
  docwire_log_func();
  if (std::holds_alternative<tag::start_processing>(info.tag))
  {
    docwire_log_var(m_data.get());
    Info info{m_data.get()};
    emit(info);
  }
  else
    emit(info);
}
