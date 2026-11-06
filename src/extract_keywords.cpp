/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "extract_keywords.h"

#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace df0
{
namespace openai
{

template<typename auditor_type>
extract_keywords<auditor_type>::extract_keywords(
	unsigned int max_keywords,
	const std::string& api_key,
	model model,
	float temperature,
	image_detail image_detail,
	audit::auditor_ref<auditor_type> auditor)
	: chat<auditor_type>(
		"Your task is to identify and extract " +
			std::to_string(max_keywords) +
			" most important keywords or key phrases from every message. The goal is to capture the most relevant and significant terms within the text.",
		api_key, model, temperature, image_detail, auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{
		return
			"max_keywords"_v = max_keywords,
			"model"_v = model,
			"temperature"_v = temperature,
			"image_detail"_v = image_detail;
	}};
}

template class DF0_OPENAI_EXPORT extract_keywords<df0::audit::null_auditor>;
template class DF0_OPENAI_EXPORT extract_keywords<df0::audit::json_stream_auditor>;

} // namespace openai
} // namespace df0
