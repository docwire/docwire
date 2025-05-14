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

#ifndef DOCWIRE_SCOPED_STACK_PUSH_H
#define DOCWIRE_SCOPED_STACK_PUSH_H

#include <stack>

namespace docwire::scoped
{

template <typename T>
class stack_push
{
public:
	stack_push(std::stack<T>& stack, const T& value) : m_stack(stack)
	{
		m_stack.push(value);
	}

	stack_push(std::stack<T>& stack, T&& value) : m_stack(stack)
	{
		m_stack.push(std::move(value));
	}

	~stack_push()
	{
		m_stack.pop();
	}

private:
	std::stack<T>& m_stack;
};

} // namespace docwire

#endif
