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

template <typename T>
class with_pimpl_owner;

template <typename T>
class with_pimpl
{
protected:
	using impl_type = pimpl_impl<T>;
	template <typename... Args>
	explicit with_pimpl(Args&&... args)
		: m_impl(std::make_unique<impl_type>(std::forward<Args>(args)...))
	{
		set_impl_owner();
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

	impl_type& impl() { return *m_impl; }
	const impl_type& impl() const { return *m_impl; }

	void renew_impl()
	{
		m_impl = std::make_unique<impl_type>();
		set_impl_owner();
	}

	void destroy_impl()
	{
		m_impl.reset();
	}

private:
	std::unique_ptr<impl_type> m_impl;

	void set_impl_owner()
	{
		if constexpr (std::is_base_of_v<with_pimpl_owner<T>, impl_type>)
			m_impl->set_owner(static_cast<T*>(this));
	}
};

template <typename T>
class with_pimpl_owner
{
protected:
	T& owner() { return *static_cast<T*>(m_owner); }
	const T& owner() const { return *static_cast<const T*>(m_owner); }
	void set_owner(T* owner) { m_owner = owner; }

private:
	T* m_owner;
	friend with_pimpl<T>;
};

} // namespace docwire

#endif
