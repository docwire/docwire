/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#include "xml_stream.h"

#include "exception.h"
#include <iostream>
#include <libxml/xmlreader.h>
#include "pthread.h"

#undef XML_STREAM_DEBUG

static size_t xml_parser_usage_counter = 0;
static pthread_mutex_t xml_parser_usage_counter_mutex = PTHREAD_MUTEX_INITIALIZER;

static void checkXmlMemGet()
{
	//in windows 64 bit there is a problem with initialization of some pointers to functions.
	//to make sure xmlMemGet will not be called twice I'm protecting this code with mutexes.
	pthread_mutex_lock(&xml_parser_usage_counter_mutex);
	if (!xmlFree)
		xmlMemGet(&xmlFree, &xmlMalloc, &xmlRealloc, NULL);
	pthread_mutex_unlock(&xml_parser_usage_counter_mutex);
}

static void initXmlParser()
{
	pthread_mutex_lock(&xml_parser_usage_counter_mutex);
	if (xml_parser_usage_counter == 0)
		xmlInitParser();
	xml_parser_usage_counter++;
	pthread_mutex_unlock(&xml_parser_usage_counter_mutex);
}

static void cleanupXmlParser()
{
	pthread_mutex_lock(&xml_parser_usage_counter_mutex);
	xml_parser_usage_counter--;
	if (xml_parser_usage_counter == 0)
		xmlCleanupParser();
	pthread_mutex_unlock(&xml_parser_usage_counter_mutex);
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
			throw doctotext::Exception("Cannot initialize XmlStream: xmlReaderForMemory has failed");
		}
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			if (impl->m_manage_xml_parser)
				cleanupXmlParser();
			xmlFreeTextReader(impl->m_reader);
			delete impl;
			impl = NULL;
			throw doctotext::Exception("Cannot initialize XmlStream: xmlTextReaderRead has failed");
		}
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		#endif
		impl->m_curr_depth = xmlTextReaderDepth(impl->m_reader);
		if (impl->m_curr_depth == -1)
		{
			if (impl->m_manage_xml_parser)
				cleanupXmlParser();
			xmlFreeTextReader(impl->m_reader);
			delete impl;
			impl = NULL;
			throw doctotext::Exception("Cannot initialize XmlStream: xmlTextReaderDepth has failed");
		}
		#ifdef XML_STREAM_DEBUG
			std::cerr << "Starting curr_depth: " << impl->m_curr_depth << "\n";
		#endif
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
	#ifdef XML_STREAM_DEBUG
	std::cerr << "# next(). curr_depth=" << impl->m_curr_depth << "\n";
	#endif
	do
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			#ifdef XML_STREAM_DEBUG
				std::cerr << "# End of file or error - Null\n";
			#endif
			impl->m_badbit = true;
			return;
		}
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		#endif
		if (xmlTextReaderDepth(impl->m_reader) < impl->m_curr_depth)
		{
			impl->m_badbit = true;
			#ifdef XML_STREAM_DEBUG
				std::cerr << "# End of level or error - Null\n";
			#endif
			return;
		}
	} while (xmlTextReaderNodeType(impl->m_reader) == 15 || xmlTextReaderDepth(impl->m_reader) > impl->m_curr_depth);
	#ifdef XML_STREAM_DEBUG
		if (xmlTextReaderConstValue(impl->m_reader) == NULL)
			std::cerr << "# null value.\n";
		else
			std::cerr << "# value:" << (char*)xmlTextReaderConstValue(impl->m_reader) << "\n";
	#endif
	impl->m_badbit = false;
}

void XmlStream::levelDown()
{
	impl->m_curr_depth++;
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# levelDown(). curr_depth=" << impl->m_curr_depth << "\n";
	#endif
	// warning TODO: <a></a> is not empty according to xmlTextReaderIsEmptyElement(). Check if it is a problem.
	if (xmlTextReaderIsEmptyElement(impl->m_reader) != 0)
	{
		impl->m_badbit = true;
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# Empty or error - Null\n";
		#endif
		return;
	}
	do
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			impl->m_badbit = true;
			#ifdef XML_STREAM_DEBUG
				std::cerr << "# End of document - Null\n";
			#endif
			return;
		}
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		#endif
		if (xmlTextReaderDepth(impl->m_reader) < impl->m_curr_depth)
		{
			impl->m_badbit = true;
			#ifdef XML_STREAM_DEBUG
				std::cerr << "# Level empty or error - Null\n";
			#endif
			return;
		}
	} while (xmlTextReaderNodeType(impl->m_reader) == 15);
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# name:" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		if (xmlTextReaderConstValue(impl->m_reader) == NULL)
			std::cerr << "# null value.\n";
		else
			std::cerr << "# value:" << (char*)xmlTextReaderConstValue(impl->m_reader) << "\n";
	#endif
}

void XmlStream::levelUp()
{
	impl->m_curr_depth--;
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# levelDown(). curr_depth=" << impl->m_curr_depth << "\n";
	#endif
	if (impl->m_badbit)
	{
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# Was null - now invalid.\n";
		#endif
		return;
	}
	for(;;)
	{
		if (xmlTextReaderRead(impl->m_reader) != 1)
		{
			impl->m_badbit = true;
			#ifdef XML_STREAM_DEBUG
				std::cerr << "# End of document or error - Null\n";
			#endif
			return;
		}
		#ifdef XML_STREAM_DEBUG
			std::cerr << "# read. type=" << xmlTextReaderNodeType(impl->m_reader) << ", depth=" << xmlTextReaderDepth(impl->m_reader) << ", name=" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		#endif
		if (xmlTextReaderNodeType(impl->m_reader) == 15 && xmlTextReaderDepth(impl->m_reader) == impl->m_curr_depth)
		{
			impl->m_badbit = false;
			break;
		}
	}
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# name:" << (char*)xmlTextReaderConstLocalName(impl->m_reader) << "\n";
		if (xmlTextReaderConstValue(impl->m_reader) == NULL)
			std::cerr << "# null value.\n";
		else
			std::cerr << "# value:" << (char*)xmlTextReaderConstValue(impl->m_reader) << "\n";
	#endif
}

char* XmlStream::content()
{
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# content()\n";
	#endif
	return (char*)xmlTextReaderConstValue(impl->m_reader);
}

std::string XmlStream::name()
{
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# name()\n";
	#endif
	return (char*)xmlTextReaderConstLocalName(impl->m_reader);
}

std::string XmlStream::fullName()
{
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# fullName()\n";
	#endif
	return (char*)xmlTextReaderConstName(impl->m_reader);
}

std::string XmlStream::stringValue()
{
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# stringValue()\n";
	#endif
	if (xmlTextReaderNodeType(impl->m_reader) != 1)
	{
		#ifdef XML_STREAM_DEBUG
			std::cerr << "!!! Getting string value not from start tag.\b";
		#endif
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
	#ifdef XML_STREAM_DEBUG
		std::cerr << "# attribute()\n";
	#endif
	if (xmlTextReaderNodeType(impl->m_reader) != 1)
	{
		#ifdef XML_STREAM_DEBUG
			std::cerr << "!!! Getting attribute not from start tag.\b";
		#endif
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
