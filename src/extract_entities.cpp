/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "extract_entities.h"

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
extract_entities<auditor_type>::extract_entities(
	const std::string& api_key,
	model model,
	float temperature,
	image_detail image_detail,
	audit::auditor_ref<auditor_type> auditor)
	: chat<auditor_type>(
		"Your task is to find all dates, timespans, addresses, companies and person names inside every message. Show it as formatted list.",
		api_key, model, temperature, image_detail, auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{
		return
			"model"_v = model,
			"temperature"_v = temperature,
			"image_detail"_v = image_detail;
	}};
}

template class DF0_OPENAI_EXPORT extract_entities<df0::audit::null_auditor>;
template class DF0_OPENAI_EXPORT extract_entities<df0::audit::json_stream_auditor>;

} // namespace openai
} // namespace df0
