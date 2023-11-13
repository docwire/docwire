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

#ifndef DOCWIRE_XML_FIXER_H
#define DOCWIRE_XML_FIXER_H

#include <string>

namespace docwire
{

class XmlFixer
{
	private:
		struct Implementation;
		Implementation* Impl;

	public:
		XmlFixer();
		~XmlFixer();
		std::string fix(const std::string& xml) const;
};

} // namespace docwire

#endif
