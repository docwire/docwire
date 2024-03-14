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

#include "output.h"

#include "exception.h"
#include <fstream>

namespace docwire
{

class Output::Implementation
{
public:

  Implementation(std::ostream &out_stream)
    : m_out_stream(&out_stream)
  {}

  Implementation(const Implementation &other)
    : m_out_stream(other.m_out_stream)
  {}

  Implementation(const Implementation &&other)
    : m_out_stream(other.m_out_stream)
  {}

  std::ostream* m_out_stream;
};

Output::Output(std::ostream &out_stream)
{
  impl = std::unique_ptr<Implementation>{new Implementation{out_stream}};
}

Output::Output(std::ostream&& out_stream)
{
  impl = std::unique_ptr<Implementation>{new Implementation{out_stream}};
}

Output::Output(const Output &other)
  : impl(new Implementation(*other.impl))
{}

Output::Output(const Output &&other)
  : impl(new Implementation(*other.impl))
{}

Output::~Output()
{
}

void
Output::process(Info &info) const
{
	if (info.tag_name != StandardTag::TAG_FILE)
		throw LogicError("Only TAG_FILE tags are supported by Output chain element");
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	if(!path && !stream)
		throw LogicError("No path or stream in TAG_FILE");
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	*impl->m_out_stream << in_stream->rdbuf();
	if (path)
		delete in_stream;
}

} // namespace docwire
