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

#include "xml_reader.h"

#include "checked.h"
#include "log_scope.h"
#include "log_entry.h"
#include "not_null.h"
#include "throw_if.h"
#include <cstddef>
#include <libxml/xmlreader.h>
#include <limits>
#include <mutex>
#include "ranged.h"
#include "serialization_enum.h" // IWYU pragma: keep

namespace docwire::xml
{

namespace
{

std::mutex xml_parser_init_mutex; // Mutex for thread-safe libxml2 initialization

static int to_libxml_parse_options(reader_blanks blanks_option)
{
	int libxml_options = 0; // XML_PARSE_NOBLANKS is a libxml2 constant
	if (blanks_option == reader_blanks::ignore)
		libxml_options |= XML_PARSE_NOBLANKS;

	return libxml_options;
}

class lib_xml2_init_and_cleanup final
{
	public:
		lib_xml2_init_and_cleanup() { xmlInitParser(); }
		~lib_xml2_init_and_cleanup() { xmlCleanupParser(); }
};

static std::unique_ptr<xmlTextReader, decltype(&xmlFreeTextReader)> make_xml_text_reader_safely(std::string_view xml, reader_blanks blanks_option)
{
	std::lock_guard<std::mutex> xml_parser_init_mutex_lock(xml_parser_init_mutex);
	static lib_xml2_init_and_cleanup init_and_cleanup{};
	const int final_options = to_libxml_parse_options(blanks_option) | XML_PARSE_NOERROR | XML_PARSE_NOWARNING;
	throw_if (xml.size() > static_cast<size_t>(std::numeric_limits<int>::max()), "XML input too large for libxml2");
	return std::unique_ptr<xmlTextReader, decltype(&xmlFreeTextReader)>(
		xmlReaderForMemory(xml.data(), static_cast<int>(xml.size()), nullptr, nullptr, final_options),
		&xmlFreeTextReader);
}

} // anonymous namespace

} // namespace docwire::xml

namespace docwire
{
template<safety_policy safety_level>
struct pimpl_impl<xml::reader<safety_level>> : pimpl_impl_base
{
	mutable not_null<std::unique_ptr<xmlTextReader, void (*)(xmlTextReaderPtr)>, safety_level> m_reader{nullptr, &xmlFreeTextReader};
    // This buffer holds the last string allocated by libxml2 for string_value().
    // This avoids re-allocating a std::string on every call.
    mutable checked<std::unique_ptr<xmlChar, void (*)(void*)>, safety_level> m_string_value_buffer{nullptr, xmlFree};
    std::exception_ptr m_callback_exception;

    pimpl_impl(std::string_view xml_sv, xml::reader_blanks blanks_option)
        : m_reader(xml::make_xml_text_reader_safely(xml_sv, blanks_option))
    {
		log::scope _{ "xml_sv"_v = xml_sv, "blanks_option"_v = blanks_option };
    }

	std::string_view name() const
	{
		const xmlChar* val = xmlTextReaderConstLocalName(m_reader.get());
		return val ? reinterpret_cast<const char*>(val) : "";
	}

	non_negative<int, safety_level> depth() const
	{
		log::scope _{};
		const at_least<-1, int, safety_level> api_result{xmlTextReaderDepth(m_reader.get())};
		const non_negative<int, safety_level> valid_result{api_result};
		return valid_result;
	}

	xml::node_type type() const
	{
		const ranged<-1, 17, int, safety_level> api_result{xmlTextReaderNodeType(m_reader.get())};
		const ranged<0, 17, int, safety_level> valid_result{api_result};
		return static_cast<xml::node_type>(valid_result.get());
	}

	bool should_skip() const
	{
		return type() == xml::node_type::processing_instruction;
	}

	bool read_next() const
	{
		do
		{
			const ranged<-1, 1, int, safety_level> api_result{xmlTextReaderRead(m_reader.get())};

			if (api_result == 0) // EOF
				return false;

			if (api_result == -1 && m_callback_exception)
				std::rethrow_exception(m_callback_exception);

			throw_if(api_result != 1, "xmlTextReaderRead failed", errors::uninterpretable_data{});
			log_entry(type(), depth().get(), name());
		}
		while (should_skip());
		return true;
	}
};
} // namespace docwire

namespace docwire::xml
{

template<safety_policy safety_level>
reader<safety_level>::reader(std::string_view xml_sv, reader_blanks blanks_option)
	: with_pimpl<reader<safety_level>>(xml_sv, blanks_option) {}

template<safety_policy safety_level>
bool reader<safety_level>::read_next() const
{
	return impl().read_next();
}

template<safety_policy safety_level>
std::string_view reader<safety_level>::content() const
{
	const xmlChar* val = xmlTextReaderConstValue(impl().m_reader.get());
	return val ? reinterpret_cast<const char*>(val) : "";
}

template<safety_policy safety_level>
std::string_view reader<safety_level>::name() const
{
	return impl().name();
}

template<safety_policy safety_level>
std::string_view reader<safety_level>::full_name() const
{
	const xmlChar* val = xmlTextReaderConstName(impl().m_reader.get());
	return val ? reinterpret_cast<const char*>(val) : "";
}

template<safety_policy safety_level>
std::string_view reader<safety_level>::string_value() const
{
	// xmlTextReaderReadString returns the content of an element as a single string.
	// It's the correct streaming equivalent of xmlNodeListGetString.
	impl().m_string_value_buffer.reset(xmlTextReaderReadString(impl().m_reader.get()));
	if (!impl().m_string_value_buffer)
		return "";
	return (const char*)impl().m_string_value_buffer.get();
}

template<safety_policy safety_level>
bool reader<safety_level>::move_to_first_attribute() const
{
	const ranged<-1, 1, int, safety_level> api_result{xmlTextReaderMoveToFirstAttribute(impl().m_reader.get())};
	const ranged<0, 1, int, safety_level> valid_result{api_result};
	return valid_result == 1;
}

template<safety_policy safety_level>
bool reader<safety_level>::move_to_next_attribute() const
{
	const ranged<-1, 1, int, safety_level> api_result{xmlTextReaderMoveToNextAttribute(impl().m_reader.get())};
	const ranged<0, 1, int, safety_level> valid_result{api_result};
	return valid_result == 1;
}

template<safety_policy safety_level>
void reader<safety_level>::move_to_element() const noexcept(safety_level == relaxed)
{
	// This is a cleanup operation. A failure indicates a logic error in an iterator, not a runtime error.
	const ranged<-1, 1, int, safety_level> api_result{xmlTextReaderMoveToElement(impl().m_reader.get())};
	[[maybe_unused]] const ranged<0, 1, int, safety_level> valid_result{api_result};
}

template<safety_policy safety_level>
non_negative<int, safety_level> reader<safety_level>::depth() const
{
	return impl().depth();
}

template<safety_policy safety_level>
node_type reader<safety_level>::type() const
{
	return impl().type();
}

template class DOCWIRE_XML_EXPORT reader<strict>;
template class DOCWIRE_XML_EXPORT reader<relaxed>;

}
