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

#ifndef DOCWIRE_TRANSCRIBE_H
#define DOCWIRE_TRANSCRIBE_H

#include "chain_element.h"

namespace docwire
{
namespace openai
{

class DllExport Transcribe : public ChainElement, public with_pimpl<Transcribe>
{
public:
	Transcribe(const std::string& api_key);
	Transcribe(Transcribe&&);
	virtual ~Transcribe();

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Info
	* @param info
	*/
	void process(Info& info) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	using with_pimpl<Transcribe>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_TRANSCRIBE_H
