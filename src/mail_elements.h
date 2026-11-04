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

#ifndef DF0_MAIL_ELEMENTS_H
#define DF0_MAIL_ELEMENTS_H

#include <cstddef>
#include <cstdint>
#include "file_extension.h"
#include "core_export.h"
#include <optional>
#include <string>

namespace df0
{
namespace mail
{

struct mail
{
  std::optional<std::string> subject;
  std::optional<std::uint32_t> date;
  std::optional<int> level;
};

struct close_mail {};

struct mail_body {};
struct close_mail_body { };

struct attachment
{
  std::optional<std::string> name;
  std::size_t size;
  std::optional<file_extension> extension;
};

struct close_attachment { };

struct folder
{
  std::optional<std::string> name;
  std::optional<int> level;
};

struct close_folder { };

} // namespace mail
} // namespace df0

#endif // DF0_MAIL_ELEMENTS_H
