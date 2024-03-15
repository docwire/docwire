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

#include "exception.h"
#include <iostream>
#include <libxml/xmlreader.h>
#include "log.h"
#include <mutex.h>

namespace docwire
{

static size_t xml_parser_usage_counter = 0;

namespace
{
	std::mutex xml_parser_usage_counter_mutex;
} // anonymous namespace

static void checkXmlMemGet()
{
	//in windows 64 bit there is a problem with initialization of some pointers to functions.
	//to make sure xmlMemGet will not be called twice I'm protecting this code with mutexes.
	std::lock_guard<std::mutex> xml_parser_usage_counter_mutex_lock(xml_parser_usage_counter_mutex);
	if (!xmlFree)
		xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
}

static void initXmlParser()
{
	std::lock_guard<std::mutex> xml_parser_usage_counter_mutex_lock(xml_parser_usage_counter_mutex);
	if (xml_parser_usage_counter == 0)
		xmlInitParser();
	xml_parser_usage_counter++;
}

static void cleanupXmlParser()
{
	std::lock_guard<std::mutex> xml_parser_usage_counter_mutex_lock(xml_parser_usage_counter_mutex);
	xml_parser_usage_counter--;
	if (xml_parser_usage_counter == 0)
		xmlCleanupParser();
}

// warning TODO: Maybe it will be good direction if XmlStream will not require loading whole xml data at once? For now\
	if we want to parse one of OOXML/ODF/ODFXML files we need to load whole file into memory at once. It would be good\
	if XmlStream was more frugal

struct XmlStream::Implementation
{
	bool m_badbit;
	xmlTextReaderPtr m_reader;
	int m_curr_depth;
	bool m_manage_xml_parser;
};

XmlStream::XmlStream(const std::string &xml, bool manage_xml_parser, int xml_parse_options)
{
	impl = NULL;
	try
	{
		impl = new Implementation;
		impl->m_reader = NULL;
		impl->m_badbit = false;
		impl->m_manage_xml_parser = manage_xml_parser;
		checkXmlMemGet();
		if (impl->m_manage_xml_parser)
			initXmlParser();
		impl->m_reader = xmlReaderForMemory(xml.c_str(), xml.length(), NULL, NULL, xml_parse_options);
		if (impl->m_reader == NULL)
		{
			if (impl->m_manage_xml_parser)
				cleanupXmlParser();
			delete impl;
			impl = NULL;
			throw RuntimeError("Cannot initialize XmlStream: xmlReaderForMemory has failed");
		}
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			if (impl->m_manage_xml_parser)
				cleanupXmlParser();
			xmlFreeTextReader(impl->m_reader);
			delete impl;
			impl = NULL;
			throw RuntimeError("Cannot initialize XmlStream: xmlTextReaderRead has failed");
		}
		docwire_log(debug) << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader);
		impl->m_curr_depth = xmlTextReaderDepth(impl->m_reader);
		if (impl->m_curr_depth == -1)
		{
			if (impl->m_manage_xml_parser)
				cleanupXmlParser();
			xmlFreeTextReader(impl->m_reader);
			delete impl;
			impl = NULL;
			throw RuntimeError("Cannot initialize XmlStream: xmlTextReaderDepth has failed");
		}
		docwire_log(debug) << "Starting curr_depth: " << impl->m_curr_depth;
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_reader != NULL)
				xmlFreeTextReader(impl->m_reader);
			delete impl;
		}
		throw;
	}
}

XmlStream::~XmlStream()
{
	if (impl->m_reader != NULL)
		xmlFreeTextReader(impl->m_reader);
	if (impl->m_manage_xml_parser)
		cleanupXmlParser();
	delete impl;
}

XmlStream::operator bool()
{
	return !impl->m_badbit;
}

void XmlStream::next()
{
	docwire_log(debug) << "# next(). curr_depth=" << impl->m_curr_depth;
	do
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			docwire_log(debug) << "# End of file or error - Null";
			impl->m_badbit = true;
			return;
		}
		docwire_log(debug) << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader);
		if (xmlTextReaderDepth(impl->m_reader) < impl->m_curr_depth)
		{
			impl->m_badbit = true;
			docwire_log(debug) << "# End of level or error - Null";
			return;
		}
	} while (xmlTextReaderNodeType(impl->m_reader) == 15 || xmlTextReaderDepth(impl->m_reader) > impl->m_curr_depth);
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl->m_reader) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl->m_reader)
		);
	impl->m_badbit = false;
}

void XmlStream::levelDown()
{
	impl->m_curr_depth++;
	docwire_log(debug) << "# levelDown(). curr_depth=" << impl->m_curr_depth;
	// warning TODO: <a></a> is not empty according to xmlTextReaderIsEmptyElement(). Check if it is a problem.
	if (xmlTextReaderIsEmptyElement(impl->m_reader) != 0)
	{
		impl->m_badbit = true;
		docwire_log(debug) << "# Empty or error - Null";
		return;
	}
	do
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			impl->m_badbit = true;
			docwire_log(debug) << "# End of document - Null";
			return;
		}
		docwire_log(debug) << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader);
		if (xmlTextReaderDepth(impl->m_reader) < impl->m_curr_depth)
		{
			impl->m_badbit = true;
			docwire_log(debug) << "# Level empty or error - Null";
			return;
		}
	} while (xmlTextReaderNodeType(impl->m_reader) == 15);
	docwire_log(debug) << "# name:" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl->m_reader) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl->m_reader)
		);
}

void XmlStream::levelUp()
{
	impl->m_curr_depth--;
	docwire_log(debug) << "# levelDown(). curr_depth=" << impl->m_curr_depth;
	if (impl->m_badbit)
	{
		docwire_log(debug) << "# Was null - now invalid.";
		return;
	}
	for(;;)
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			impl->m_badbit = true;
			docwire_log(debug) << "# End of document or error - Null";
			return;
		}
		docwire_log(debug) << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader);
		if (xmlTextReaderNodeType(impl->m_reader) == 15 && xmlTextReaderDepth(impl->m_reader) == impl->m_curr_depth)
		{
			impl->m_badbit = false;
			break;
		}
	}
	docwire_log(debug) << "# name:" << (char*)xmlTextReaderConstLocalName(impl->m_reader);
	docwire_log(debug) << (
		xmlTextReaderConstValue(impl->m_reader) == NULL ?
			std::string("# null value.") :
			std::string("# value:") + (char*)xmlTextReaderConstValue(impl->m_reader)
		);
}

char* XmlStream::content()
{
	docwire_log(debug) << "# content()";
	return (char*)xmlTextReaderConstValue(impl->m_reader);
}

std::string XmlStream::name()
{
	docwire_log(debug) << "# name()";
	return (char*)xmlTextReaderConstLocalName(impl->m_reader);
}

std::string XmlStream::fullName()
{
	docwire_log(debug) << "# fullName()";
	return (char*)xmlTextReaderConstName(impl->m_reader);
}

std::string XmlStream::stringValue()
{
	docwire_log(debug) << "# stringValue()";
	if (xmlTextReaderNodeType(impl->m_reader) != 1)
	{
		docwire_log(debug) << "!!! Getting string value not from start tag.";
		return "";
	}
	xmlNodePtr node = xmlTextReaderExpand(impl->m_reader);
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
	if (xmlTextReaderNodeType(impl->m_reader) != 1)
	{
		docwire_log(debug) << "!!! Getting attribute not from start tag.";
		return "";
	}
	xmlNodePtr node = xmlTextReaderExpand(impl->m_reader);
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
