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

#include "xml_stream.h"

#include <boost/algorithm/string/trim.hpp>
#include "libxml/globals.h"
#include <libxml/xmlreader.h>
#include "log.h"
#include <mutex>
#include "throw_if.h"

namespace docwire
{

template <>
std::string stringify(const xmlError& error)
{
	return boost::trim_right_copy(std::string{error.message}) +
		std::string(" (code: ") + std::to_string(error.code) + ")";
}

static size_t xml_parser_usage_counter = 0;

namespace
{

std::mutex xml_parser_init_mutex;

/**
 * @brief Class for managing the initialization and cleanup of the libxml2 parser.
 * 
 * This class ensures that the libxml2 parser is initialized and cleaned up.
 * Initialization is performed when the class object is constructed, and
 * cleanup is performed when the class object is destroyed.
 * 
 * Object of this class should be created before any XML processing is performed, and
 * exists until all XML processing is finished.
 * 
 * @warning You would like to make a static global object of this class probably but be
 * aware that libxml2 also have some static initialization that should be performed first
 * so static local object will be probably better.
*/
class LibXml2InitAndCleanup
{
	public:
		LibXml2InitAndCleanup()
		{
			xmlInitParser();
		}

		~LibXml2InitAndCleanup()
		{
			xmlCleanupParser();
		}
};

/**
 * Creates a unique pointer to an xmlTextReader object safely.
 *
 * This function safely creates a unique pointer to an xmlTextReader object, ensuring proper memory management and thread safety.
 * It provides a safer and more efficient way to handle the creation and destruction of xmlTextReader objects compared to directly using xmlReaderForMemory. 
 * It leverages std::unique_ptr for automatic memory management and introduces a locking mechanism for thread safety.
 * libxml2 is initialized if needed and cleaned up on application exit.
 *
 * @param xml The XML string to be parsed.
 * @param xml_parse_options The options for parsing the XML.
 *
 * @return A unique pointer to an xmlTextReader object. Ownership is transferred to the caller.
 */
static std::unique_ptr<xmlTextReader, decltype(&xmlFreeTextReader)> make_xml_text_reader_safely(const std::string& xml, int xml_parse_options)
{
	// libxml2 requires one-time initialization via xmlInitParser(). While functions like
	// xmlReaderForMemory can trigger this initialization implicitly, that is not thread-safe.
	// We explicitly call xmlInitParser() within a lock to ensure safe, one-time initialization.
	std::lock_guard<std::mutex> xml_parser_init_mutex_lock(xml_parser_init_mutex);
	// Init libxml2 on first use, but cleanup at application exit not to interfere with other threads or
	// other code that uses libxml2.
	static LibXml2InitAndCleanup init_and_cleanup{};
	return std::unique_ptr<xmlTextReader, decltype(&xmlFreeTextReader)>(
		xmlReaderForMemory(xml.c_str(), xml.length(), NULL, NULL,
			xml_parse_options | XML_PARSE_NOERROR | XML_PARSE_NOWARNING),
		&xmlFreeTextReader);
}

} // anonymous namespace

// warning TODO: Maybe it will be good direction if XmlStream will not require loading whole xml data at once? For now\
	if we want to parse one of OOXML/ODF/ODFXML files we need to load whole file into memory at once. It would be good\
	if XmlStream was more frugal

template<>
struct pimpl_impl<XmlStream> : pimpl_impl_base
{
	bool m_badbit = false;
	std::unique_ptr<xmlTextReader, decltype(&xmlFreeTextReader)> m_reader;
	int m_curr_depth;

	pimpl_impl(const std::string& xml, int xml_parse_options)
		: m_reader(make_xml_text_reader_safely(xml, xml_parse_options))
	{
		throw_if (m_reader == NULL, "Cannot initialize xmlTextReader");
		throw_if (!read_next(), "Cannot initialize xmlTextReader");
		m_curr_depth = xmlTextReaderDepth(m_reader.get());
		throw_if (m_curr_depth == -1, "Cannot initialize xmlTextReader");
		docwire_log(debug) << "Starting curr_depth: " << m_curr_depth;
	}

	bool should_skip() const
	{
		return xmlTextReaderNodeType(m_reader.get()) == XML_READER_TYPE_PROCESSING_INSTRUCTION;
	}

	bool read_next()
	{
		do
		{
			int read_status = xmlTextReaderRead(m_reader.get());
			throw_if (read_status == -1, "xmlTextReaderRead failed", *xmlGetLastError());
			if (read_status == 0) // eof
				return false;
			throw_if (read_status != 1, "Incorrect xmlTextReader status code", read_status);
			docwire_log(debug) << "# read. type=" << xmlTextReaderNodeType(m_reader.get()) << ", depth=" << xmlTextReaderDepth(m_reader.get()) << ", name=" << (char*)xmlTextReaderConstLocalName(m_reader.get());
		}
		while (should_skip());
		return true;
	}
};

XmlStream::XmlStream(const std::string &xml, int xml_parse_options)
	: with_pimpl(xml, xml_parse_options)
{
}

XmlStream::operator bool()
{
	return !impl().m_badbit;
}

void XmlStream::next()
{
	docwire_log(debug) << "# next(). curr_depth=" << impl().m_curr_depth;
	do
	{
		if (!impl().read_next())
		{
			docwire_log(debug) << "# End of file or error - Null";
			impl().m_badbit = true;
			return;
		}
		if (xmlTextReaderDepth(impl().m_reader.get()) < impl().m_curr_depth)
		{
			impl().m_badbit = true;
			docwire_log(debug) << "# End of level or error - Null";
			return;
		}
	} while (
		xmlTextReaderNodeType(impl().m_reader.get()) == XML_READER_TYPE_END_ELEMENT ||
		xmlTextReaderDepth(impl().m_reader.get()) > impl().m_curr_depth);
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl().m_reader.get()) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl().m_reader.get())
		);
	impl().m_badbit = false;
}

void XmlStream::levelDown()
{
	impl().m_curr_depth++;
	docwire_log(debug) << "# levelDown(). curr_depth=" << impl().m_curr_depth;
	// warning TODO: <a></a> is not empty according to xmlTextReaderIsEmptyElement(). Check if it is a problem.
	if (xmlTextReaderIsEmptyElement(impl().m_reader.get()) != 0)
	{
		impl().m_badbit = true;
		docwire_log(debug) << "# Empty or error - Null";
		return;
	}
	do
	{
		if (!impl().read_next())
		{
			impl().m_badbit = true;
			docwire_log(debug) << "# End of document - Null";
			return;
		}
		if (xmlTextReaderDepth(impl().m_reader.get()) < impl().m_curr_depth)
		{
			impl().m_badbit = true;
			docwire_log(debug) << "# Level empty or error - Null";
			return;
		}
	} while (xmlTextReaderNodeType(impl().m_reader.get()) == XML_READER_TYPE_END_ELEMENT);
	docwire_log(debug) << "# name:" << (char*)xmlTextReaderConstLocalName(impl().m_reader.get()) << "\n";
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl().m_reader.get()) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl().m_reader.get())
		);
}

void XmlStream::levelUp()
{
	impl().m_curr_depth--;
	docwire_log(debug) << "# levelDown(). curr_depth=" << impl().m_curr_depth;
	if (impl().m_badbit)
	{
		docwire_log(debug) << "# Was null - now invalid.";
		return;
	}
	for(;;)
	{
		if (!impl().read_next())
		{
			impl().m_badbit = true;
			docwire_log(debug) << "# End of document or error - Null";
			return;
		}
		if (xmlTextReaderNodeType(impl().m_reader.get()) == XML_READER_TYPE_END_ELEMENT &&
			xmlTextReaderDepth(impl().m_reader.get()) == impl().m_curr_depth)
		{
			impl().m_badbit = false;
			break;
		}
	}
	docwire_log(debug) << "# name:" << (char*)xmlTextReaderConstLocalName(impl().m_reader.get());
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl().m_reader.get()) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl().m_reader.get())
		);
}

char* XmlStream::content()
{
	docwire_log(debug) << "# content()";
	return (char*)xmlTextReaderConstValue(impl().m_reader.get());
}

std::string XmlStream::name()
{
	docwire_log(debug) << "# name()";
	return (char*)xmlTextReaderConstLocalName(impl().m_reader.get());
}

std::string XmlStream::fullName()
{
	docwire_log(debug) << "# fullName()";
	return (char*)xmlTextReaderConstName(impl().m_reader.get());
}

std::string XmlStream::stringValue()
{
	docwire_log(debug) << "# stringValue()";
	if (xmlTextReaderNodeType(impl().m_reader.get()) != 1)
	{
		docwire_log(debug) << "!!! Getting string value not from start tag.";
		return "";
	}
	xmlNodePtr node = xmlTextReaderExpand(impl().m_reader.get());
	if (node == NULL)
		return "";
	xmlChar* val = xmlNodeListGetString(node->doc, node->xmlChildrenNode, 1);
	if (val == NULL)
		return "";
	std::string s((char*)val);
	xmlFree(val);
	return s;
}

std::string XmlStream::attribute(const std::string& attr_name)
{
	docwire_log(debug) << "# attribute()";
	if (xmlTextReaderNodeType(impl().m_reader.get()) != 1)
	{
		docwire_log(debug) << "!!! Getting attribute not from start tag.";
		return "";
	}
	xmlNodePtr node = xmlTextReaderExpand(impl().m_reader.get());
	if (node == NULL)
		return "";
	xmlChar* attr = xmlGetProp(node, (xmlChar*)attr_name.c_str());
	if (attr == NULL)
		return "";
	std::string s((char*)attr);
	xmlFree(attr);
	return s;
}

}; // namespace docwire
