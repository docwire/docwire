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

#ifndef DOCWIRE_LOCAL_AI_MODEL_CHAIN_ELEMENT_H
#define DOCWIRE_LOCAL_AI_MODEL_CHAIN_ELEMENT_H

#include "chain_element.h"
#include "error_tags.h"
#include "model_runner.h"
#include "throw_if.h"

namespace docwire::local_ai
{

/**
 * @brief A model chain element that processes input text using a model runner.
 *
 * This class is a chain element that takes a prompt and a model runner. It
 * processes the input by appending the prompt to the input text and then
 * passing the text to the model runner. The output of the model runner is
 * then emitted as a new Info object.
 */
class model_chain_element : public ChainElement
{
public:
	/**
	 * @brief Construct a model chain element.
	 *
	 * @param prompt The prompt to append to the input text.
	 * @param model_runner The model runner to use for processing the text.
	 */
	model_chain_element(const std::string& prompt, std::shared_ptr<model_runner> model_runner)
		: m_prompt{prompt}, m_model_runner{model_runner}
	{}

	/**
	 * @brief Destroy the model chain element.
	 */
	virtual ~model_chain_element()
	{}

	/**
	 * @brief Process the input.
	 *
	 * If the input is not a data source, emit the input and return. If the
	 * input is a data source, append the prompt to the input text and then
	 * pass the text to the model runner. The output of the model runner is
	 * then emitted as a new Info object.
	 *
	 * @param info The input Info object to process.
	 */
	void process(Info &info) const override
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

	/**
	 * @brief Check if the model chain element is a leaf.
	 *
	 * The model chain element is never a leaf, so this function always returns
	 * false.
	 *
	 * @return false Always false.
	 */
	bool is_leaf() const override
	{
		return false;
	}

private:
	std::string m_prompt;
	std::shared_ptr<model_runner> m_model_runner;
};

} // namespace docwire::local_ai

#endif // DOCWIRE_AI_MODEL_CHAIN_ELEMENT_H
