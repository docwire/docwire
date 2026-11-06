/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_OPENAI_EMBED_H
#define DF0_OPENAI_EMBED_H

#include "pipeline/element_base.hpp"
#include "openai_export.h"
#include "message.h"
#include "pimpl.h"
#include "audit/auditor_ref.hpp"
#include "audit/with_auditor.hpp"
#include <concepts>
#include <string>

namespace df0
{
namespace openai
{

enum class embed_model
{
	/// text-embedding-3-small: A new third generation embedding model, is OpenAI recommended default for most use cases.
	text_embedding_3_small,
	/// text-embedding-3-large: A larger and more powerful third generation embedding model, is OpenAI best performing embedding model.
	text_embedding_3_large,
	/// text-embedding-ada-002: Older generation of OpenAI's embedding model.
	text_embedding_ada_002,
};

template<typename auditor_type = audit::null_auditor>
class DF0_OPENAI_EXPORT embed
    : public pipeline::transformer_element<embed<auditor_type>>,
      public with_pimpl<embed<auditor_type>>,
      private audit::with_auditor<auditor_type>
{
public:
	/**
	 * @brief Constructs the component with the default stateless auditing policy.
	 *
	 * This overload is available only when the configured auditor is stateless
	 * and therefore default-constructible.
	 *
	 * @tparam stateless_auditor_type Injected auditor type selected by this overload.
	 */
	template<audit::stateless_auditor stateless_auditor_type = auditor_type>
	embed(
		const std::string& api_key,
		embed_model model = embed_model::text_embedding_3_small)
		: embed(api_key, model, audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit embed(
		const std::string& api_key,
		embed_model model,
		audit::auditor_ref<auditor_type> auditor);

	/**
	* @brief Executes transform operation for given node data.
	* @see df0::message_ptr
	* @param tag
	* @param callback
	*/
	continuation operator()(message_ptr msg, const message_callbacks& emit_message);

protected:
	using audit::with_auditor<auditor_type>::auditor;

private:
	using with_pimpl<embed<auditor_type>>::impl;
};

// Deduction guide for CTAD
embed(const std::string&, embed_model) -> embed<audit::null_auditor>;

} // namespace openai
} // namespace df0

#endif //DF0_OPENAI_EMBED_H
