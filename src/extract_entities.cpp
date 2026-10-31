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

#include "extract_entities.h"

#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/json_stream_auditor.hpp"
#include "audit/scope.hpp"
#include "audit/with_auditor.hpp"
#include "named.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace docwire
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

template class DOCWIRE_OPENAI_EXPORT extract_entities<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT extract_entities<docwire::audit::json_stream_auditor>;

} // namespace openai
} // namespace docwire
