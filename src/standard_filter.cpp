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

#include "mail_elements.h"

namespace docwire
{

message_transform_func StandardFilter::filterByFolderName(const std::vector<std::string> &names)
{  
  return [names](message_ptr msg, const message_callbacks& emit_message) -> continuation
  {
    if (!msg->is<mail::Folder>())
      return emit_message(std::move(msg));
    auto folder_name = msg->get<mail::Folder>().name;
    if (folder_name)
    {
      if (!std::any_of(names.begin(), names.end(), [&folder_name](const std::string &name){return (*folder_name) == name;}))
      {
        return continuation::skip;
      }
    }
    return emit_message(std::move(msg));
  };
}

message_transform_func StandardFilter::filterByAttachmentType(const std::vector<file_extension>& types)
{
  return [types](message_ptr msg, const message_callbacks& emit_message) -> continuation
  {
    if (!msg->is<mail::Attachment>())
      return emit_message(std::move(msg));
    if (!msg->is<mail::Attachment>() || !msg->get<mail::Attachment>().extension)
      return emit_message(std::move(msg));
    auto attachment_type = msg->get<mail::Attachment>().extension;
    if (attachment_type)
    {
      if (!std::any_of(types.begin(), types.end(), [&attachment_type](const file_extension& type){ return (*attachment_type) == type; }))
      {
        return continuation::skip;
      }
    }
    return emit_message(std::move(msg));
  };
}

message_transform_func StandardFilter::filterByMailMinCreationTime(unsigned int min_time)
{
  return [min_time](message_ptr msg, const message_callbacks& emit_message) -> continuation
  {
	  if (!msg->is<mail::Mail>())
		  return emit_message(std::move(msg));
	  auto mail_creation_time = msg->get<mail::Mail>().date;
	  if (mail_creation_time)
	  {
		  if (*mail_creation_time < min_time)
		  {
			  return continuation::skip;
		  }
	  }
	  return emit_message(std::move(msg));
  };
}

message_transform_func StandardFilter::filterByMailMaxCreationTime(unsigned int max_time)
{
	return [max_time](message_ptr msg, const message_callbacks& emit_message) -> continuation
	{
		if (!msg->is<mail::Mail>())
			return emit_message(std::move(msg));
		auto mail_creation_time = msg->get<mail::Mail>().date;
		if (mail_creation_time)
		{
			if (*mail_creation_time > max_time)
			{
				return continuation::skip;
			}
		}
		return emit_message(std::move(msg));
	};
}

message_transform_func StandardFilter::filterByMaxNodeNumber(unsigned int max_nodes_arg)
{
  return [max_nodes = max_nodes_arg, node_no = 0](message_ptr msg, const message_callbacks& emit_message) mutable -> continuation
  {
	  if (node_no++ == max_nodes)
	  {
		  return continuation::stop;
	  }
	  return emit_message(std::move(msg));
  };
}

} // namespace docwire
