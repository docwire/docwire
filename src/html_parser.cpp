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

#include "html_parser.h"

#include <algorithm>
#include <boost/signals2.hpp>
#include "data_stream.h"
#include "exception.h"
#include "entities.h"
#include "htmlcxx/html/Node.h"
#include "htmlcxx/html/ParserSax.h"
#include "htmlcxx/html/CharsetConverter.h"
#include <list>
#include "metadata.h"
#include "misc.h"
#include <iostream>
#include "charsetdetect.h"

using namespace htmlcxx::HTML;

class DocToTextSaxParser : public ParserSax
{
	private:
		std::vector<Link>& m_links;
		std::string& m_text;
		std::string& m_html_content;
		const FormattingStyle& m_formatting;
		bool m_in_title;
		bool m_in_style;
		bool m_first_in_seg; // do not insert paragraph breaks if we are in the beginning of document, paragraph or table cell
		bool m_in_link;
		bool m_in_script;
		bool m_in_list;
		bool m_in_table;
		bool m_turn_off_ul_enumeration;
		bool m_turn_off_ol_enumeration;
		std::string m_style_text;
		std::string m_curr_link_url;
		std::string m_curr_link_text;
		std::string* m_curr_text_block;
		std::string m_charset;
		std::ostream& m_log_stream;
		bool m_verbose_logging;
		htmlcxx::CharsetConverter* m_converter;
		char* m_decoded_buffer;	//for decoding html entities
		size_t m_decoded_buffer_size;
		bool m_skip_decoding;
    boost::signals2::signal<void(doctotext::Info &info)> m_on_new_node_signal;

		struct HtmlContainer
		{
			std::string m_curr_text;

			virtual ~HtmlContainer()= default;
			virtual void endRow(){}
			virtual void endLine(){}
			virtual void endCol(){}

			virtual std::vector<svector>* getRows()
			{
				return nullptr;
			}

			virtual std::vector<std::string>* getLines()
			{
				return nullptr;
			}

			virtual bool isList()
			{
				return false;
			}

			virtual bool isTable()
			{
				return false;
			}

			//methods only for lists:
			virtual bool isVisibleListEnumeration()
			{
				return false;
			}

			virtual void showListEnumeration(bool show){}
		};

		struct HtmlTable : public HtmlContainer
		{
			svector m_curr_row_cells;
			std::vector<svector> m_rows;

			bool isTable() override
			{
				return true;
			}

			std::vector<svector>* getRows() override
			{
				return &m_rows;
			}

			void endCol() override
			{
				m_curr_row_cells.push_back(m_curr_text);
				m_curr_text.clear();
			}

			void endRow() override
			{
				m_rows.push_back(m_curr_row_cells);
				m_curr_row_cells.clear();
			}
		};

		struct HtmlList : public HtmlContainer
		{
			std::vector<std::string> m_lines;
			bool m_show_list_enumeration;

			HtmlList() : m_show_list_enumeration(true){}

			bool isList() override
			{
				return true;
			}

			std::vector<std::string>* getLines() override
			{
				return &m_lines;
			}

			void endLine() override
			{
				m_lines.push_back(m_curr_text);
				m_curr_text.clear();
			}

			bool isVisibleListEnumeration() override
			{
				return m_show_list_enumeration;
			}

			void showListEnumeration(bool show) override
			{
				m_show_list_enumeration = show;
			}
		};

		std::list<HtmlContainer*> m_html_containers;
		HtmlContainer* m_curr_html_container;

		void addTextToCurrBlock(const std::string& text)
		{
			*m_curr_text_block += text;
			m_first_in_seg = false;
		}

		bool lastIsSpaceInCurrBlock()
		{
			return m_curr_text_block->length() > 0 && std::isspace(static_cast<unsigned char>(*m_curr_text_block->rbegin()));
		}

		static std::string normalizeSpaces(const std::string& s, bool prev_is_space)
		{
			std::string normalized;
			bool prev_space = prev_is_space;
			for (char i : s)
				if (std::isspace(static_cast<unsigned char>(i)))
				{
					if (!prev_space)
					{
						normalized += ' ';
						prev_space = true;
					}
				}
				else
				{
					normalized += i;
					prev_space = false;
				}
			return normalized;
		}

		void convertToUtf8(std::string& text)
		{
			if (!m_skip_decoding)
			{
				if (m_converter)
					text = m_converter->convert(text);
			}
			// warning TODO: Check if libxml2 provides such a functionality. Similar function in html library does not work for some entities
			if (m_decoded_buffer_size < text.length() * 2)
			{
				delete[] m_decoded_buffer;
				m_decoded_buffer = nullptr;
			}
			if (!m_decoded_buffer)
			{
				m_decoded_buffer = new char[text.length() * 2];
				m_decoded_buffer_size = text.length() * 2;
			}
			size_t size = decode_html_entities_utf8(m_decoded_buffer, text.data());
			text = std::string(m_decoded_buffer, size);
		}

		void createCharsetConverter()
		{
			if (!m_skip_decoding && m_converter == nullptr && m_charset != "utf-8" && m_charset != "UTF-8")
			{
				try
				{
					m_converter = new htmlcxx::CharsetConverter(m_charset, "UTF-8");
				}
				catch (htmlcxx::CharsetConverter::Exception& ex)
				{
					m_log_stream << "Warning: Cant convert text to UTF-8 from " + m_charset;
          delete m_converter;
					m_converter = nullptr;
				}
			}
		}

		static void parseStyleAttributes(const std::string& style, std::map<std::string, std::string>& attributes)
		{
			//  TODO: I have not found similar function in htmlcxx, but my searches were perfunctory
			bool reading_name = true, reading_value = false;
			std::string name, value;
			for (char i : style)
			{
				if (std::isspace(static_cast<unsigned char>(i)))
					continue;
				if (reading_name)
				{
					if (i == ':')
					{
						reading_name = false;
						reading_value = true;
						continue;
					}
					name += i;
				}
				else if (reading_value)
				{
					if (i == ';')
					{
						reading_name = true;
						reading_value = false;
						attributes[name] = value;
						continue;
					}
					value += i;
				}
			}
		}

		void parseCSS()
		{
			// warning TODO: CSS selector in htmlcxx is definitely not thread-safe. For now, we only need to know
			// about one attribute (list-style). This function only fixes the problem with automatic HTML files
			// produced by LibreOffice/OpenOffice. Those programs use <ol> and <ul>, but usually turn off
			// automatic list enumeration (list-style: none), and use own enumeration. So we have a small problem.
			// We can obtain output like this: "1. 1. first", "2. 2. second" etc. We should repair htmlcxx or find something
			// better if we encounter a bigger problem in the future. This function is really primitive.

			//we should look for something like: ol/ul/li { list-style: none }
			size_t search_position = 0;
			size_t found_position = 0;
			while ((found_position = m_style_text.find("list-style:", search_position)) != std::string::npos)
			{
				search_position = found_position + 11;
				//first, obtain style name:
				std::string style_name;
				for (size_t i = found_position + 11; i < m_style_text.length(); ++i, ++search_position)
				{
					if (m_style_text[i] == ';')
						break;
					if (!std::isspace(static_cast<unsigned char>(m_style_text[i])))
						style_name += m_style_text[i];
				}
				//now obtain which tags this rule (list-style) concerns.
				if (style_name == "none")
				{
					std::string match;
					int match_index = 0;
					for (size_t i = found_position; i > 0; --i)
					{
						if (m_style_text[i] == '{')
						{
							match_index = static_cast<int>(i) - 1;
							break;
						}
					}
					while (match_index >= 0)
					{
						if (std::isspace(static_cast<unsigned char>(m_style_text[match_index])))
						{
							if (match.length() > 0)
								break;
						}
						else
							match = m_style_text[match_index] + match;
						--match_index;
					}
					if (match == "li")
					{
						m_turn_off_ol_enumeration = true;
						m_turn_off_ul_enumeration = true;
					}
					else if (match == "ul")
						m_turn_off_ul_enumeration = true;
					else if (match == "ol")
						m_turn_off_ol_enumeration = true;
				}
			}
		}

	protected:
		void foundTag(Node node, bool isEnd) override
		{
			if (isEnd)
			{
				if (strcasecmp(node.tagName().c_str(), "title") == 0)
					m_in_title = false;
				else if (strcasecmp(node.tagName().c_str(), "style") == 0)
				{
					m_in_style = false;
					parseCSS();
				}
				else if (!m_in_list && strcasecmp(node.tagName().c_str(), "div") == 0)
				{
					if (m_text.length() > 0 && m_text[m_text.length() - 1] != '\n')
						addTextToCurrBlock("\n");
				}
				else if (strcasecmp(node.tagName().c_str(), "p") == 0)
				{
					if (!m_first_in_seg)
					{
						if (!m_in_list)
							addTextToCurrBlock("\n");
						m_first_in_seg = true;
					}
				}
				else if (strcasecmp(node.tagName().c_str(), "a") == 0 && m_in_link)
				{
          m_in_link = false;
          if (m_curr_html_container)
            m_curr_text_block = &m_curr_html_container->m_curr_text;
          else
            m_curr_text_block = &m_text;
          if (m_curr_link_url.length() > 0) {
            std::string link_text = formatUrl(m_curr_link_url, m_curr_link_text, m_formatting);
            convertToUtf8(link_text);
            Link link(m_curr_link_url, link_text, 0);
            m_links.push_back(link);
            std::string text;
            insertSpecialLinkBlockIntoText(text, link);
            addTextToCurrBlock(text);
          } else {
            std::string url = formatUrl(m_curr_link_url, m_curr_link_text, m_formatting);
            addTextToCurrBlock(url);
          }
          addTextToCurrBlock(" ");
        }
				else if (strcasecmp(node.tagName().c_str(), "table") == 0 && m_in_table)
				{
					if (m_html_containers.size() > 1)
					{
						//last_container is not "last" yet (current last object is going to be deleted soon, but we still need it)
						HtmlContainer* last_container = *(++(m_html_containers.rbegin()));
						m_in_list = last_container->isList();
						m_in_table = last_container->isTable();
						m_curr_text_block = &last_container->m_curr_text;
						addTextToCurrBlock(formatTable(*m_curr_html_container->getRows(), m_formatting));
						m_html_containers.pop_back();
						delete m_curr_html_container;
						m_curr_html_container = m_html_containers.back();
					}
					else
					{
						m_in_table = false;
						m_curr_text_block = &m_text;
						addTextToCurrBlock(formatTable(*m_curr_html_container->getRows(), m_formatting));
						m_html_containers.pop_back();
						delete m_curr_html_container;
						m_curr_html_container = nullptr;
					}
				}
				else if (strcasecmp(node.tagName().c_str(), "tr") == 0 && m_in_table)
				{
					m_curr_html_container->endRow();
				}
				else if (strcasecmp(node.tagName().c_str(), "span") == 0 ||
						 strcasecmp(node.tagName().c_str(), "button") == 0)
				{
					addTextToCurrBlock(" ");
				}
				else if ((strcasecmp(node.tagName().c_str(), "td") == 0 || strcasecmp(node.tagName().c_str(), "th") == 0) && m_in_table)
				{
					m_curr_html_container->endCol();
				}
				else if ((strcasecmp(node.tagName().c_str(), "script") == 0 || strcasecmp(node.tagName().c_str(), "iframe") == 0) && m_in_script)
				{
					m_in_script = false;
				}
				else if (m_in_list && (strcasecmp(node.tagName().c_str(), "ul") == 0
						 || strcasecmp(node.tagName().c_str(), "ol") == 0))
				{
					bool is_ordered = node.tagName() == "ol";
					if (m_html_containers.size() > 1)
					{
						//last_container is not "last" yet (current last object is going to be deleted soon, but we still need it)
						HtmlContainer* last_container = *(++(m_html_containers.rbegin()));
						m_in_list = last_container->isList();
						m_in_table = last_container->isTable();
						m_curr_text_block = &last_container->m_curr_text;
					}
					else
					{
						m_in_list = false;
						m_curr_text_block = &m_text;
					}
					if (m_curr_html_container->isVisibleListEnumeration())
					{
						if (!is_ordered)
							addTextToCurrBlock(formatList(*m_curr_html_container->getLines(), m_formatting));
						else
							addTextToCurrBlock(formatNumberedList(*m_curr_html_container->getLines()));
					}
					else
					{
						FormattingStyle formatting;
						formatting.list_style.setPrefix("");
						addTextToCurrBlock(formatList(*m_curr_html_container->getLines(), formatting));
					}
					m_html_containers.pop_back();
					delete m_curr_html_container;
					m_curr_html_container = nullptr;
					if (!m_html_containers.empty())
						m_curr_html_container = m_html_containers.back();
					addTextToCurrBlock("\n");
				}
				else if (strcasecmp(node.tagName().c_str(), "br") == 0)
				{
					addTextToCurrBlock("\n");
				}
				else if (m_in_list && (strcasecmp(node.tagName().c_str(), "li") == 0))
				{
					m_curr_html_container->endLine();
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "title") == 0)
				m_in_title = true;
			else if (strcasecmp(node.tagName().c_str(), "style") == 0)
				m_in_style = true;
			else if (strcasecmp(node.tagName().c_str(), "p") == 0)
			{
				if (!m_first_in_seg)
				{
					if (!m_in_list)
						addTextToCurrBlock("\n");
					m_first_in_seg = true;
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "ol") == 0
					 || strcasecmp(node.tagName().c_str(), "ul") == 0)
			{
				node.parseAttributes();
				addTextToCurrBlock("\n");
				m_in_list = true;
				m_in_table = false;
				m_curr_html_container = nullptr;
				try
				{
					m_curr_html_container = new HtmlList;
					std::pair<bool, std::string> style = node.attribute("style");
					std::map<std::string, std::string> style_attributes;
					if (style.first)
						parseStyleAttributes(style.second, style_attributes);
					if (style_attributes.find("list-style") == style_attributes.end())
					{
						if (node.tagName() == "ol" && m_turn_off_ol_enumeration)
							m_curr_html_container->showListEnumeration(false);
						else if (node.tagName() == "ul" && m_turn_off_ul_enumeration)
							m_curr_html_container->showListEnumeration(false);
					}
					else
					{
						if (style_attributes["list-style"] == "none")
							m_curr_html_container->showListEnumeration(false);
					}
					m_html_containers.push_back(m_curr_html_container);
				}
				catch (std::bad_alloc& ba)
				{
					if (m_curr_html_container)
						delete m_curr_html_container;
					m_curr_html_container = nullptr;
					throw;
				}
				m_curr_text_block = &m_curr_html_container->m_curr_text;
			}
			else if (strcasecmp(node.tagName().c_str(), "br") == 0)
			{
				addTextToCurrBlock("\n");
			}
			else if (strcasecmp(node.tagName().c_str(), "script") == 0 || strcasecmp(node.tagName().c_str(), "iframe") == 0)
			{
				m_in_script = true;
			}
			else if (strcasecmp(node.tagName().c_str(), "a") == 0)
			{
				node.parseAttributes();
				std::pair<bool, std::string> href = node.attribute("href");
				if (href.first && href.second.length() > 0)
				{
					m_in_link = true;
					m_curr_link_url = href.second;
					//skip target if begins with "#" or "javascript"
					if (m_curr_link_url[0] == '#' || m_curr_link_url.find("javascript") == 0)
						m_curr_link_url = "";
					else
						convertToUtf8(m_curr_link_url);
					m_curr_link_text = "";
					m_curr_text_block = &m_curr_link_text;
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "img") == 0)
			{
				node.parseAttributes();
				std::pair<bool, std::string> title = node.attribute("alt");
				if (title.first)
				{
					convertToUtf8(title.second);
					addTextToCurrBlock(title.second + "\n");
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "table") == 0)
			{
				m_in_list = false;
				m_in_table = true;
				m_curr_html_container = nullptr;
				try
				{
					m_curr_html_container = new HtmlTable;
					m_html_containers.push_back(m_curr_html_container);
				}
				catch (std::bad_alloc& ba)
				{
					if (m_curr_html_container)
						delete m_curr_html_container;
					m_curr_html_container = nullptr;
					throw;
				}
				m_curr_text_block = &m_curr_html_container->m_curr_text;
			}
			else if (strcasecmp(node.tagName().c_str(), "body") == 0)
			{
				//if we still don't have an encoding, we can try guess it.
				if (m_charset.empty())
				{
					csd_t charset_detector = csd_open();
					if (charset_detector == (csd_t)-1)
					{
						m_log_stream << "Warning: Could not create charset detector\n";
					}
					else
					{
						const char* res = nullptr;
						try
						{
							csd_consider(charset_detector, m_html_content.c_str(), static_cast<int>(m_html_content.length()));
							res = csd_close(charset_detector);
							charset_detector = (csd_t)-1;
						}
						catch (std::bad_alloc& ba)
						{
							//maybe bad_alloc can be thrown inside csd_consider, in that case, make sure charset_detector is closed
							if (charset_detector != (csd_t)-1)
								csd_close(charset_detector);
							throw;
						}
						if (res != nullptr)
						{
							m_charset = std::string(res);
							if (m_verbose_logging)
								m_log_stream << "Could not found explicit information about encoding. Estimated encoding: " + m_charset + "\n";
							createCharsetConverter();
						}
					}
					//if we still don't know which encoding is used...
					if (m_charset.empty())
					{
						if (m_verbose_logging)
							m_log_stream << "Could not detect encoding. Document is assumed to be encoded in UTF-8\n";
						m_charset = "UTF-8";
					}
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "meta") == 0)
			{
				//meta can contain information about encoding. If we still don't have information about it,
				//maybe this tag will show us encoding for this document.
				if (m_charset.empty())
				{
					node.parseAttributes();
					//it can be something like: <meta content="text/html; charset=utf-8">...
					std::pair<bool, std::string> content = node.attribute("content");
					if (content.first)
					{
						size_t charset_pos = content.second.find("charset");
						if (charset_pos != std::string::npos)
						{
							charset_pos += 7;
							while (charset_pos < content.second.length())
							{
								if (content.second[charset_pos] == ';')
									break;
								if (!std::isspace(static_cast<unsigned char>(content.second[charset_pos])) && content.second[charset_pos] != '=')
									m_charset += content.second[charset_pos];
								++charset_pos;
							}
						}
					}
					else
					{
						//or maybe it is <meta charset="encoding">...
						std::pair<bool, std::string> charset = node.attribute("charset");
						if (charset.first)
						{
							m_charset = charset.second;
						}
					}
					if (!m_charset.empty())
					{
						if (m_verbose_logging)
							m_log_stream << "Following encoding was detected: " + m_charset + "\n";
						createCharsetConverter();
					}
				}
			}
		}

		void foundText(Node node) override
		{
			if (m_in_style)
			{
				m_style_text += node.text();
				return;
			}
			if (m_in_title || m_in_script)
				return;
			std::string text = normalizeSpaces(node.text(), lastIsSpaceInCurrBlock());
			convertToUtf8(text);
			addTextToCurrBlock(text);
		}

		void endParsing() override
		{
			decodeSpecialLinkBlocks(m_text, m_links, m_log_stream);
		}

		void beginParsing() override
		{
			//if this is xhtml document, information about encoding may be stored between <?xml and ?>.
			//htmlcxx seems not to parse this fragment, so I should do this manually
			size_t initial_xml_start_pos = m_html_content.find("<?xml");
			size_t initial_xml_end_pos = m_html_content.find("?>");
			if (initial_xml_start_pos != std::string::npos && initial_xml_end_pos != std::string::npos
					&& initial_xml_end_pos > initial_xml_start_pos && initial_xml_end_pos < m_html_content.length())
			{
				std::string initial_xml = m_html_content.substr(initial_xml_start_pos, initial_xml_end_pos - initial_xml_start_pos);
				std::transform(initial_xml.begin(), initial_xml.end(), initial_xml.begin(), ::tolower);

				size_t encoding_pos = initial_xml.find("encoding");
				if (encoding_pos != std::string::npos)
				{
					encoding_pos += 7;
					while (encoding_pos < initial_xml.length() && initial_xml[encoding_pos] != '\"')
						++encoding_pos;
					++encoding_pos;
					while (encoding_pos < initial_xml.length() && initial_xml[encoding_pos] != '\"')
					{
						m_charset += initial_xml[encoding_pos];
						++encoding_pos;
					}
					if (!m_charset.empty())
					{
						if (m_verbose_logging)
							m_log_stream << "Following encoding was detected: " + m_charset + "\n";
						createCharsetConverter();
					}
				}
			}
		}

	public:
		DocToTextSaxParser(std::string& html_content, std::string& text, const FormattingStyle& formatting, std::ostream& log_stream, std::vector<Link>& links, bool verbose_logging, bool skip_decoding)
			: m_text(text), m_formatting(formatting), m_in_title(false), m_in_style(false), m_in_link(false), m_converter(nullptr), m_decoded_buffer(nullptr),
			  m_first_in_seg(false), m_in_script(false), m_in_list(false), m_log_stream(log_stream), m_links(links), m_decoded_buffer_size(0),
			  m_curr_html_container(nullptr), m_curr_text_block(&m_text), m_in_table(false), m_turn_off_ul_enumeration(false), m_turn_off_ol_enumeration(false),
			  m_html_content(html_content), m_verbose_logging(verbose_logging), m_skip_decoding(skip_decoding)
		{
		}

		~DocToTextSaxParser() override
		{
			if (m_decoded_buffer)
				delete[] m_decoded_buffer;
			if (m_converter)
				delete m_converter;
			auto it = m_html_containers.begin();
			while (it != m_html_containers.end())
			{
				delete *it;
				++it;
			}
			m_html_containers.clear();
		}
};

class DocToTextMetaSaxParser : public ParserSax
{
	private:
		Metadata& m_meta;
		std::ostream& m_log_stream;

	protected:
		void foundTag(Node node, bool isEnd) override
		{
			if (strcasecmp(node.tagName().c_str(), "meta") == 0)
			{
				node.parseAttributes();
				std::pair<bool, std::string> name = node.attribute("name");
				std::pair<bool, std::string> content = node.attribute("content");
				// author, changedby, created, changed - LibreOffice 3.5
				// dcterms - old OpenOffice.org
				if (strcasecmp(name.second.c_str(), "author") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.creator") == 0)
					m_meta.setAuthor(content.second);
				else if (strcasecmp(name.second.c_str(), "changedby") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.contributor") == 0)
				{
					// Multiple changedby meta tags are possible - LibreOffice 3.5 is an example
					if (std::string(m_meta.lastModifiedBy()).empty())
						m_meta.setLastModifiedBy(content.second);
				}
				else if (strcasecmp(name.second.c_str(), "created") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.issued") == 0)
				{
					tm creation_date;
					if (string_to_date(content.second, creation_date))
					{
					  m_meta.setCreationDate(creation_date);
					}
				}
				else if (strcasecmp(name.second.c_str(), "changed") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.modified") == 0)
				{
					// Multiple changed meta tags are possible - LibreOffice 3.5 is an example
					if (m_meta.lastModificationDate().tm_year == 0)
					{
						tm last_modification_date;
						if (string_to_date(content.second, last_modification_date))
						{
							m_meta.setLastModificationDate(last_modification_date);
						}
					}
				}
			}
		}

	public:
		DocToTextMetaSaxParser(Metadata& meta, std::ostream& log_stream)
			: m_meta(meta), m_log_stream(log_stream)
		{
		};
};

struct HTMLParser::Implementation
{
	bool m_skip_decoding{};
	std::string m_file_name;
	bool m_verbose_logging{};
	std::ostream* m_log_stream{};
	DataStream* m_data_stream{};
	std::vector<Link> m_links;
  boost::signals2::signal<void(doctotext::Info &info)> m_on_new_node_signal;
};

HTMLParser::HTMLParser(const std::string& file_name, const std::shared_ptr<doctotext::ParserManager> &inParserManager)
: Parser(inParserManager)
{
	impl = nullptr;
	try
	{
		impl = new Implementation();
		impl->m_skip_decoding = false;
		impl->m_file_name = file_name;
		impl->m_verbose_logging = false;
		impl->m_log_stream = &std::cerr;
		impl->m_data_stream = nullptr;
		impl->m_data_stream = new FileStream(file_name);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

HTMLParser::HTMLParser(const char *buffer, size_t size, const std::shared_ptr<doctotext::ParserManager> &inParserManager)
: Parser(inParserManager)
{
	impl = nullptr;
	try
	{
		impl = new Implementation();
		impl->m_skip_decoding = false;
		impl->m_file_name = "Memory buffer";
		impl->m_verbose_logging = false;
		impl->m_log_stream = &std::cerr;
		impl->m_data_stream = nullptr;
		impl->m_data_stream = new BufferStream(buffer, size);
	}
	catch (std::bad_alloc& ba)
	{
		if (impl)
		{
			if (impl->m_data_stream)
				delete impl->m_data_stream;
			delete impl;
		}
		throw;
	}
}

HTMLParser::~HTMLParser()
{
	if (impl->m_data_stream)
		delete impl->m_data_stream;
	delete impl;
}

void HTMLParser::setVerboseLogging(bool verbose)
{
	impl->m_verbose_logging = verbose;
}

void HTMLParser::setLogStream(std::ostream& log_stream)
{
	impl->m_log_stream = &log_stream;
}

Parser&
HTMLParser::withParameters(const doctotext::ParserParameters &parameters)
{
	doctotext::Parser::withParameters(parameters);
	impl->m_verbose_logging = isVerboseLogging();
	impl->m_log_stream = &getLogOutStream();
	return *this;
}

bool HTMLParser::isHTML()
{
	if (!impl->m_data_stream->open())
		throw Exception("Error opening file " + impl->m_file_name);
	size_t size = impl->m_data_stream->size();
	std::string content(size, 0);
	if (!impl->m_data_stream->read(&content[0], sizeof(unsigned char), size))
		throw Exception("Error reading file " + impl->m_file_name);
	impl->m_data_stream->close();
	return content.find("<html") != std::string::npos || content.find("<HTML") != std::string::npos;
}

void HTMLParser::getLinks(std::vector<Link>& links)
{
	links = impl->m_links;
}

std::string HTMLParser::plainText(const FormattingStyle& formatting) const
{
	if (!impl->m_data_stream->open())
		throw Exception("Error opening file " + impl->m_file_name);
	size_t size = impl->m_data_stream->size();
	std::string content(size, 0);
	if (!impl->m_data_stream->read(&content[0], sizeof(unsigned char), size))
		throw Exception("Error reading file " + impl->m_file_name);
	impl->m_data_stream->close();
	std::string text;
	DocToTextSaxParser parser(content, text, formatting, *impl->m_log_stream, impl->m_links, impl->m_verbose_logging, impl->m_skip_decoding);
	parser.parse(content);
  doctotext::Info info(StandardTag::TAG_TEXT, text);
  impl->m_on_new_node_signal(info);
	return text;
}

void
HTMLParser::parse() const
{
	if (isVerboseLogging())
			getLogOutStream() << "Using HTML parser.\n";

  auto formatting_style = m_parameters.getParameterValue<FormattingStyle>("formatting_style");
  if (formatting_style)
  {
    plainText(*formatting_style);
  }
  else
  {
    FormattingStyle formatting;
    formatting.url_style = URL_STYLE_EXTENDED;
    plainText(formatting);
  }
  Metadata metadata = metaData();
  doctotext::Info metadata_info(StandardTag::TAG_METADATA, "", metadata.getFieldsAsAny());
  impl->m_on_new_node_signal(metadata_info);
}

doctotext::Parser&
HTMLParser::addOnNewNodeCallback(doctotext::NewNodeCallback callback)
{
  impl->m_on_new_node_signal.connect(callback);
  return *this;
}

Metadata HTMLParser::metaData() const
{
	*impl->m_log_stream << "Extracting metadata.\n";
	Metadata meta;
	if (!impl->m_data_stream->open())
		throw Exception("Error opening file " + impl->m_file_name);
	size_t size = impl->m_data_stream->size();
	std::string content(size, 0);
	if (!impl->m_data_stream->read(&content[0], sizeof(unsigned char), size))
		throw Exception("Error reading file " + impl->m_file_name);
	impl->m_data_stream->close();
	DocToTextMetaSaxParser parser(meta, *impl->m_log_stream);
	parser.parse(content);
	return meta;
}

void HTMLParser::skipCharsetDecoding()
{
	impl->m_skip_decoding = true;
}
