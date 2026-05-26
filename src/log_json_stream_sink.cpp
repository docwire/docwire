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

#include "log_json_stream_sink.h"

#include "json_serialization.h"
#include <mutex>

namespace docwire::log
{

std::function<void(const record&)> json_stream_sink(ref_or_owned<std::ostream> stream)
{
	// This state object will be captured by the lambda. Its destructor will
	// be called at program exit, ensuring the JSON array is properly closed.
	struct stream_state
	{
		ref_or_owned<std::ostream> m_stream;
		bool m_first_log = true;
		std::mutex m_mutex;

		explicit stream_state(ref_or_owned<std::ostream> s) : m_stream(std::move(s)) {}
		~stream_state()
		{
			if (!m_first_log)
			{
                std::lock_guard lock(m_mutex);
				m_stream.get() << std::endl << "]" << std::endl;
			}
		}
	};

	auto state = std::make_shared<stream_state>(std::move(stream));

	return [state](const record& rec)
    {
		serialization::object log_record_object = create_base_metadata(rec.m_location);
		log_record_object.v["log"] = rec.m_context;
		std::string json_output = serialization::to_json(log_record_object);

		std::lock_guard lock(state->m_mutex);
		if (state->m_first_log)
		{
			state->m_stream.get() << "[" << std::endl;
			state->m_first_log = false;
		}
		else
		{
			state->m_stream.get() << "," << std::endl;
		}
		state->m_stream.get() << json_output;
	};
}

} // namespace docwire::log
