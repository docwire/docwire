/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#include "model_chain_element.h"

#include "error_tags.h"
#include "throw_if.h"

namespace docwire::local_ai
{

void model_chain_element::process(Info& info)
{
	if (!std::holds_alternative<data_source>(info.tag))
	{
		emit(info);
		return;
	}

	const data_source& data = std::get<data_source>(info.tag);
	throw_if (data.file_extension() && *data.file_extension() != file_extension{".txt"}, errors::program_logic{});
	std::string input = m_prompt + "\n" + data.string();
	std::string output = m_model_runner->process(input);

	Info new_info(data_source{output});
	emit(new_info);
}

} // namespace docwire::local_ai
