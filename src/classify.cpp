/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "classify.h"

#include "boost/algorithm/string/join.hpp"
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
classify<auditor_type>::classify(
	const std::set<std::string>& categories,
	const std::string& api_key,
	model model,
	float temperature,
	image_detail image_detail,
	audit::auditor_ref<auditor_type> auditor)
	: chat<auditor_type>(
		"Your task is to classify every message to one of the following categories: " + boost::algorithm::join(categories, ", ")
			+ ". Answer with the category name only, exactly as it was written.",
		api_key, model, temperature, image_detail, auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{
		return
			"categories"_v = categories,
			"model"_v = model,
			"temperature"_v = temperature,
			"image_detail"_v = image_detail;
	}};
}

template class DF0_OPENAI_EXPORT classify<df0::audit::null_auditor>;
template class DF0_OPENAI_EXPORT classify<df0::audit::json_stream_auditor>;

} // namespace openai
} // namespace df0
