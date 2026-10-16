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

#include "find.h"

#include "audit/auditor_ref.hpp"
#include "audit/entry.hpp"
#include "audit/legacy_log_audit.hpp"
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
find<auditor_type>::find(
	const std::string& what,
	const std::string& api_key,
	model model,
	float temperature,
	image_detail image_detail,
	audit::auditor_ref<auditor_type> auditor)
	: chat<auditor_type>(
		"Your task is to find all occurrences of \"" + what + "\" in the provided text or image.\n\n"
		"1. On the first line, write only the total count of occurrences found. If none are found, write 0.\n"
		"2. On each subsequent line, provide a brief, one-sentence description of where each occurrence is located. For example: 'A red car is parked near the building on the left.' or 'The word \"processing\" appears in the first paragraph.'\n\n"
		"Do not describe the entire image or text. Focus only on the location of what you were asked to find.",
		api_key, model, temperature, image_detail, auditor)
{
	audit::scope audit_scope{this->auditor(), [&]{
		return
			"what"_v = what,
			"model"_v = model,
			"temperature"_v = temperature,
			"image_detail"_v = image_detail;
	}};
}

template class DOCWIRE_OPENAI_EXPORT find<docwire::audit::null_auditor>;
template class DOCWIRE_OPENAI_EXPORT find<docwire::audit::legacy_log_audit>;
template class DOCWIRE_OPENAI_EXPORT find<docwire::audit::json_stream_auditor>;

} // namespace openai
} // namespace docwire
