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

#ifndef DOCWIRE_LOG_STATE_SAVER_H
#define DOCWIRE_LOG_STATE_SAVER_H

#include "log_core.h"

namespace docwire::log
{

/**
 * @brief RAII class to save and restore logging state (sink and filter).
 */
class state_saver
{
public:
	state_saver()
		: m_old_sink(get_sink()), m_old_filter(get_filter())
	{}

	// Prevent copying and moving to ensure single ownership of restoration responsibility
	state_saver(const state_saver&) = delete;
	state_saver& operator=(const state_saver&) = delete;
	state_saver(state_saver&&) = delete;
	state_saver& operator=(state_saver&&) = delete;

	~state_saver()
	{
		set_sink(m_old_sink);
		set_filter(m_old_filter);
	}

private:
	std::function<void(const record&)> m_old_sink;
	std::string m_old_filter;
};

} // namespace docwire::log

#endif // DOCWIRE_LOG_STATE_SAVER_H
