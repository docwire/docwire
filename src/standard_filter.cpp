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

#include "standard_filter.h"
namespace docwire
{

NewNodeCallback StandardFilter::filterByFolderName(const std::vector<std::string> &names)
{
  return [names](Info &info)
  {
    if (!std::holds_alternative<tag::Folder>(info.tag))
      return;
    auto folder_name = std::get<tag::Folder>(info.tag).name;
    if (folder_name)
    {
      if (!std::any_of(names.begin(), names.end(), [&folder_name](const std::string &name){return (*folder_name) == name;}))
      {
        info.skip = true;
      }
    }
  };
}

NewNodeCallback StandardFilter::filterByAttachmentType(const std::vector<std::string> &types)
{
  return [types](Info &info)
  {
    if (!std::holds_alternative<tag::Attachment>(info.tag))
      return;
    if (!std::get<tag::Attachment>(info.tag).extension)
      return;
    auto attachment_type = std::get<tag::Attachment>(info.tag).extension;
    if (attachment_type)
    {
      if (!std::any_of(types.begin(), types.end(), [&attachment_type](const std::string &type){return (*attachment_type) == type;}))
      {
        info.skip = true;
      }
    }
  };
}

NewNodeCallback StandardFilter::filterByMailMinCreationTime(unsigned int min_time)
{
  return [min_time](Info &info)
  {
    if (!std::holds_alternative<tag::Mail>(info.tag))
      return;
    auto mail_creation_time = std::get<tag::Mail>(info.tag).date;
    if (mail_creation_time)
    {
      if (*mail_creation_time < min_time)
      {
        info.skip = true;
      }
    }
  };
}

NewNodeCallback StandardFilter::filterByMailMaxCreationTime(unsigned int max_time)
{
  return [max_time](Info &info)
  {
    if (!std::holds_alternative<tag::Mail>(info.tag))
      return;
    auto mail_creation_time = std::get<tag::Mail>(info.tag).date;
    if (mail_creation_time)
    {
      if (mail_creation_time > max_time)
      {
        info.skip = true;
      }
    }
  };
}

NewNodeCallback StandardFilter::filterByMaxNodeNumber(unsigned int max_nodes)
{
  return [max_nodes, node_no = 0](Info &info) mutable
  {
    if (node_no++ == max_nodes)
    {
      info.cancel = true;
    }
  };
}
} // namespace docwire
