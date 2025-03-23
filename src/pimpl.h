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

#ifndef DOCWIRE_PIMPL_H
#define DOCWIRE_PIMPL_H

#include <memory>

namespace docwire
{

template <typename T>
struct pimpl_impl;

class with_pimpl_base {};

struct pimpl_impl_base
{
	virtual ~pimpl_impl_base() = default;
	virtual void set_owner(with_pimpl_base&)
	{
	}
};

template <typename T>
class with_pimpl_owner;

template <typename T>
class with_pimpl : public with_pimpl_base
{
protected:
	using impl_type = pimpl_impl<T>;

	template <typename... Args>
	impl_type* create_impl(Args&&... args)
	{
		if constexpr (std::is_base_of_v<with_pimpl_owner<T>, impl_type>)
		{
			static_assert(std::is_constructible_v<impl_type, T&, Args...>,
				"Template specialization of pimpl_impl<T> that inherits from with_pimpl_owner<T> is required to have constructor with T&, Args... arguments");
			return new impl_type(static_cast<T&>(*this), std::forward<Args>(args)...);
		}
		else
		{
			static_assert(std::is_constructible_v<impl_type, Args...>,
				"Template specialization of pimpl_impl<T> is required to have constructor with Args... arguments");
			return new impl_type(impl_type{std::forward<Args>(args)...});
		}
	}

	template <typename... Args>
	explicit with_pimpl(Args&&... args)
		: m_impl(static_cast<pimpl_impl_base*>(create_impl(std::forward<Args>(args)...)))
	{
	}

	with_pimpl(with_pimpl<T>&& other) noexcept
		: m_impl(std::move(other.m_impl))
	{
		if (m_impl)
			set_impl_owner();
	}

	with_pimpl(std::nullptr_t) {}

	with_pimpl& operator=(with_pimpl&& other) noexcept {
		if (this != &other)
		{
			m_impl = std::move(other.m_impl);
			if (m_impl)
				set_impl_owner();
		}
		return *this;
	}

	template <typename DeferInstantiation = void>
	impl_type& impl() { return *static_cast<impl_type*>(m_impl.get()); }

	template <typename DeferInstantiation = void>
	const impl_type& impl() const { return *static_cast<impl_type*>(m_impl.get()); }

	template <typename... Args>
	void renew_impl(Args&&... args)
	{
		m_impl.reset(static_cast<pimpl_impl_base*>(create_impl(std::forward<Args>(args)...)));
		set_impl_owner();
	}

	void destroy_impl()
	{
		m_impl.reset();
	}

private:
	std::unique_ptr<pimpl_impl_base> m_impl;

	void set_impl_owner()
	{
		m_impl->set_owner(*this);
	}
};

template <typename T>
class with_pimpl_owner : public pimpl_impl_base
{
protected:
	with_pimpl_owner(T& owner) : m_owner(owner) {}
	T& owner() { return m_owner; }
	const T& owner() const { return m_owner; }

	void set_owner(with_pimpl_base& owner) override
	{
		m_owner = static_cast<T&>(static_cast<with_pimpl<T>&>(owner));
	}

private:
	std::reference_wrapper<T> m_owner;
	friend with_pimpl<T>;
};

} // namespace docwire

#endif
