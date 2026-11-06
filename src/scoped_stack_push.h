/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_SCOPED_STACK_PUSH_H
#define DF0_SCOPED_STACK_PUSH_H

#include <stack>

namespace df0::scoped
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

} // namespace df0

#endif
