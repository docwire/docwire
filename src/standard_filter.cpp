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

tag_transform_func StandardFilter::filterByFolderName(const std::vector<std::string> &names)
{  
  return [names](Tag&& tag, const emission_callbacks& emit_tag) -> continuation
  {
    if (!std::holds_alternative<tag::Folder>(tag))
      return emit_tag(std::move(tag));
    auto folder_name = std::get<tag::Folder>(tag).name;
    if (folder_name)
    {
      if (!std::any_of(names.begin(), names.end(), [&folder_name](const std::string &name){return (*folder_name) == name;}))
      {
        return continuation::skip;
      }
    }
    return emit_tag(std::move(tag));
  };
}

tag_transform_func StandardFilter::filterByAttachmentType(const std::vector<file_extension>& types)
{
  return [types](Tag&& tag, const emission_callbacks& emit_tag) -> continuation
  {
    if (!std::holds_alternative<tag::Attachment>(tag))
      return emit_tag(std::move(tag));
    if (!std::get<tag::Attachment>(tag).extension)
      return emit_tag(std::move(tag));
    auto attachment_type = std::get<tag::Attachment>(tag).extension;
    if (attachment_type)
    {
      if (!std::any_of(types.begin(), types.end(), [&attachment_type](const file_extension& type){ return (*attachment_type) == type; }))
      {
        return continuation::skip;
      }
    }
    return emit_tag(std::move(tag));
  };
}

tag_transform_func StandardFilter::filterByMailMinCreationTime(unsigned int min_time)
{
  return [min_time](Tag&& tag, const emission_callbacks& emit_tag) -> continuation
  {
	  if (!std::holds_alternative<tag::Mail>(tag))
		  return emit_tag(std::move(tag));
	  auto mail_creation_time = std::get<tag::Mail>(tag).date;
	  if (mail_creation_time)
	  {
		  if (*mail_creation_time < min_time)
		  {
			  return continuation::skip;
		  }
	  }
	  return emit_tag(std::move(tag));
  };
}

tag_transform_func StandardFilter::filterByMailMaxCreationTime(unsigned int max_time)
{
	return [max_time](Tag&& tag, const emission_callbacks& emit_tag) -> continuation
	{
		if (!std::holds_alternative<tag::Mail>(tag))
			return emit_tag(std::move(tag));
		auto mail_creation_time = std::get<tag::Mail>(tag).date;
		if (mail_creation_time)
		{
			if (*mail_creation_time > max_time)
			{
				return continuation::skip;
			}
		}
		return emit_tag(std::move(tag));
	};
}

tag_transform_func StandardFilter::filterByMaxNodeNumber(unsigned int max_nodes_arg)
{
  return [max_nodes = max_nodes_arg, node_no = 0](Tag&& tag, const emission_callbacks& emit_tag) mutable -> continuation
  {
	  if (node_no++ == max_nodes)
	  {
		  return continuation::stop;
	  }
	  return emit_tag(std::move(tag));
  };
}

} // namespace docwire
