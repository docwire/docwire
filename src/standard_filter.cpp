/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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
    if (info.tag_name != StandardTag::TAG_FOLDER)
      return;
    auto folder_name = info.getAttributeValue<std::string>("name");
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
    if (info.tag_name != StandardTag::TAG_ATTACHMENT)
      return;
    if (info.attributes.find("extension") == info.attributes.end())
      return;
    auto attachment_type = info.getAttributeValue<std::string>("extension");
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
    if (info.tag_name != StandardTag::TAG_MAIL)
      return;
    auto mail_creation_time = info.getAttributeValue<unsigned int>("date");
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
    if (info.tag_name != StandardTag::TAG_MAIL)
      return;
    auto mail_creation_time = info.getAttributeValue<unsigned int>("date");
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
