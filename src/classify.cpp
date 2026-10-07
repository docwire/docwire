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

#include "classify.h"

#include "boost/algorithm/string/join.hpp"
#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace docwire
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

template class DOCWIRE_OPENAI_EXPORT classify<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT classify<docwire::audit::legacy_log_audit>;

} // namespace openai
} // namespace docwire
