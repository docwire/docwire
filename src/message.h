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

#ifndef DOCWIRE_MESSAGE_H
#define DOCWIRE_MESSAGE_H

#include <functional>
#include <memory>
#include <typeinfo>
#include "type_id.h"

namespace docwire
{

enum class continuation { proceed, skip, stop };
struct message_callbacks;
using message_sequence_streamer = std::function<continuation(const message_callbacks&)>;

template <typename T>
struct message;

struct message_base
{
  virtual ~message_base() = default;
  virtual type_id object_type_id() const noexcept = 0;
  virtual const std::type_info& object_type_info() const noexcept = 0;

  template <typename T>
  bool is() const noexcept
  {
    return object_type_id() == type_id_of<T>();
  }
  template <typename T>
  const T& get() const
  {
    return static_cast<const message<T>&>(*this).object;
  }

  template <typename T>
  T& get()
  {
    return static_cast<message<T>&>(*this).object;
  }
};

template <typename T>
struct message : message_base
{
  T object;
  message(T&& object) : object(std::move(object)) {}
  const std::type_info& object_type_info() const noexcept override { return typeid(T); }
  type_id object_type_id() const noexcept override
	{
		return type_id_of<T>();
	}
};

using message_ptr = std::shared_ptr<message_base>;

struct message_callbacks
{
  std::function<continuation(message_ptr)> m_further;
  std::function<continuation(message_ptr)> m_back;

  continuation further(message_ptr msg) const { return m_further(std::move(msg)); }
  
  template <typename T>
  continuation further(T&& object) const { return m_further(std::make_shared<message<T>>(std::forward<T>(object))); }

  continuation back(message_ptr msg) const { return m_back(std::move(msg)); }

  template <typename T>
  continuation back(T&& object) const { return m_back(std::make_shared<message<T>>(std::forward<T>(object))); }

  continuation operator()(message_ptr msg) const { return further(std::move(msg)); }

  template <typename T>
  continuation operator()(T&& object) const { return further(std::forward<T>(object)); }
};

} // namespace docwire

#endif //DOCWIRE_MESSAGE_H
