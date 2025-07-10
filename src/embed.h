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

#ifndef DOCWIRE_OPENAI_EMBED_H
#define DOCWIRE_OPENAI_EMBED_H

#include "chain_element.h"
#include "openai_export.h"
#include "tags.h"

namespace docwire
{
namespace openai
{

class DOCWIRE_OPENAI_EXPORT embed : public ChainElement, public with_pimpl<embed>
{
public:
	enum class model
	{
		/// text-embedding-3-small: A new third generation embedding model, is OpenAI recommended default for most use cases.
		text_embedding_3_small,
		/// text-embedding-3-large: A larger and more powerful third generation embedding model, is OpenAI best performing embedding model.
		text_embedding_3_large,
		/// text-embedding-ada-002: Older generation of OpenAI's embedding model.
		text_embedding_ada_002,
	};

	embed(const std::string& api_key, model model = model::text_embedding_3_small);

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Tag
	* @param tag
	* @param callback
	*/
	continuation operator()(Tag&& tag, const emission_callbacks& emit_tag) override;

	bool is_leaf() const override
	{
		return false;
	}

private:
	using with_pimpl<embed>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_EMBED_H
