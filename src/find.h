/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_OPENAI_FIND_H
#define DF0_OPENAI_FIND_H

#include "chat.h"
#include <concepts>

namespace df0
{
namespace openai
{

template<typename auditor_type = audit::null_auditor>
class DF0_OPENAI_EXPORT find
    : public pipeline::transformer_element<find<auditor_type>>,
      public chat<auditor_type>
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
	find(
		const std::string& what,
		const std::string& api_key,
		model model = model::gpt_5,
		float temperature = 0.0f,
		image_detail image_detail = image_detail::automatic)
		: find(what, api_key, model, temperature, image_detail,
		       audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit find(
		const std::string& what,
		const std::string& api_key,
		model model,
		float temperature,
		image_detail image_detail,
		audit::auditor_ref<auditor_type> auditor);
};

// Deduction guide for CTAD
find(const std::string&, const std::string&, model, float, image_detail) -> find<audit::null_auditor>;

} // namespace openai
} // namespace df0

#endif //DF0_OPENAI_FIND_H
