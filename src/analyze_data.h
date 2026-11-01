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

#ifndef DOCWIRE_OPENAI_ANALYZE_DATA_H
#define DOCWIRE_OPENAI_ANALYZE_DATA_H

#include "chat.h"
#include <concepts>
#include <string>

namespace docwire
{
namespace openai
{

template<typename auditor_type = audit::null_auditor>
class DOCWIRE_OPENAI_EXPORT analyze_data
    : public pipeline::transformer_element<analyze_data<auditor_type>>,
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
	analyze_data(
		const std::string& api_key,
		model model = model::gpt_5,
		float temperature = 0.0f,
		image_detail image_detail = image_detail::automatic)
		: analyze_data(api_key, model, temperature, image_detail,
		               audit::auditor_ref<stateless_auditor_type>{})
	{}

	explicit analyze_data(
		const std::string& api_key,
		model model,
		float temperature,
		image_detail image_detail,
		audit::auditor_ref<auditor_type> auditor);
};

} // namespace openai
} // namespace docwire

#endif //DOCWIRE_OPENAI_ANALYZE_DATA_H
