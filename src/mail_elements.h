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

#ifndef DOCWIRE_MAIL_ELEMENTS_H
#define DOCWIRE_MAIL_ELEMENTS_H

#include <cstddef>
#include <cstdint>
#include "file_extension.h"
#include "core_export.h"
#include <optional>
#include <string>

namespace docwire
{
namespace mail
{

struct DOCWIRE_CORE_EXPORT Mail
{
  std::optional<std::string> subject;
  std::optional<std::uint32_t> date;
  std::optional<int> level;
};

struct DOCWIRE_CORE_EXPORT CloseMail {};

struct DOCWIRE_CORE_EXPORT MailBody {};
struct DOCWIRE_CORE_EXPORT CloseMailBody { };

struct DOCWIRE_CORE_EXPORT Attachment
{
  std::optional<std::string> name;
  std::size_t size;
  std::optional<file_extension> extension;
};

struct DOCWIRE_CORE_EXPORT CloseAttachment { };

struct DOCWIRE_CORE_EXPORT Folder
{
  std::optional<std::string> name;
  std::optional<int> level;
};

struct DOCWIRE_CORE_EXPORT CloseFolder { };

} // namespace mail
} // namespace docwire

#endif // DOCWIRE_MAIL_ELEMENTS_H