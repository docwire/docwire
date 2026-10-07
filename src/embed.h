/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_OPENAI_EMBED_H
#define DOCWIRE_OPENAI_EMBED_H

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "message.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <string>

namespace docwire
{
namespace openai
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OPENAI_EXPORT embed
    : public pipeline::transformer_element<embed<auditor_type>>,
      public with_pimpl<embed<auditor_type>>,
      private audit::with_auditor<auditor_type>
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

	explicit embed(
		const std::string& api_key,
		model model = model::text_embedding_3_small,
		audit::auditor_ref<auditor_type> auditor = {});

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::message_ptr
	* @param tag
	* @param callback
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<embed<auditor_type>>::impl;
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_EMBED_H
