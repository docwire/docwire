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

#ifndef DOCWIRE_XML_STREAM_H
#define DOCWIRE_XML_STREAM_H

#include <string>

namespace docwire
{

class XmlStream
{
	private:
		struct Implementation;
		Implementation* impl;

	public:
		///Throws RuntimeError if initialization has failed.
		XmlStream(const std::string& xml, bool manage_xml_parser, int xml_parse_options = 0);
		~XmlStream();
		operator bool();
		void next();
		void levelDown();
		void levelUp();
		char* content();
		std::string name();
		std::string fullName();
		std::string stringValue();
		std::string attribute(const std::string& attr_name);
};

}; // namespace docwire

#endif
