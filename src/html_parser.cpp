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

#include "html_parser.h"

#include <algorithm>
#include <regex>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "entities.h"
#include "htmlcxx/html/Node.h"
#include "htmlcxx/html/ParserSax.h"
#include "htmlcxx/html/CharsetConverter.h"
#include "log.h"
#include "make_error.h"
#include "misc.h"
#include <mutex>
#include "charsetdetect.h"
#include <set>
#include "tags.h"

namespace docwire
{

using namespace htmlcxx::HTML;

namespace
{

struct StrCaseInsensitiveLess
{
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

typedef std::set<std::string, StrCaseInsensitiveLess> CaseInsensitiveStringSet;

bool str_iequals(const std::string& lhs, const std::string& rhs)
{
	return strcasecmp(lhs.c_str(), rhs.c_str()) == 0;
}

attributes::Styling html_node_styling(const Node& node)
{
	attributes::Styling styling;
	std::pair<bool, std::string> class_attr = node.attribute("class");
	if (class_attr.first)
	{
		boost::algorithm::split(styling.classes, class_attr.second, boost::is_any_of(" "));
	}
	std::pair<bool, std::string> id_attr = node.attribute("id");
	if (id_attr.first)
	{
		styling.id = id_attr.second;
	}
	std::pair<bool, std::string> style_attr = node.attribute("style");
	if (style_attr.first)
	{
		styling.style = style_attr.second;
	}
	return styling;
}

} // unnamed namespace

class SaxParser : public ParserSax
{
	private:
		std::string& m_html_content;
		bool m_in_title;
		bool m_in_style;
		bool m_in_script;
		bool m_turn_off_ul_enumeration;
		bool m_turn_off_ol_enumeration;
		std::string m_style_text;
		std::string m_charset;
		htmlcxx::CharsetConverter* m_converter;
		char* m_decoded_buffer;	//for decoding html entities
		size_t m_decoded_buffer_size;
		bool m_skip_decoding;
		const HTMLParser* m_parser;
		char m_last_char_in_inline_formatting_context;
		std::string m_buffered_text;

		void convertToUtf8(std::string& text)
		{
			if (!m_skip_decoding)
			{
				if (m_converter)
					text = m_converter->convert(text);
			}
			// warning TODO: Check if libxml2 provides such a functionality. Similar function in html library does not work for some entities
			if (m_decoded_buffer_size < text.length() * 2 + 1)
			{
				delete[] m_decoded_buffer;
				m_decoded_buffer = nullptr;
			}
			if (!m_decoded_buffer)
			{
				m_decoded_buffer = new char[text.length() * 2 + 1];
				m_decoded_buffer_size = text.length() * 2 + 1;
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
					m_parser->sendTag(errors::make_nested_ptr(ex, make_error("Cannot create charset to UTF-8 converter", m_charset)));
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
			const std::string tag_name = node.tagName();
			docwire_log(debug) << "HTML tag found: " << (isEnd ? "/" : "") << tag_name;
			if (!m_buffered_text.empty())
			{
				// https://developer.mozilla.org/en-US/docs/Web/HTML/Block-level_elements
				// TODO: elements can have also block style in CSS
				CaseInsensitiveStringSet block_level_elements({"address", "article", "aside", "blockquote", "details", "dialog", "dd", "div", "dl",
					"dt", "fieldset", "figcaption", "figure", "footer", "form", "h1", "h2", "h3", "h4", "h5", "h6", "header", "hgroup", "hr", "li",
					"main", "nav", "ol", "p", "pre", "section", "table", "ul"});
				// We treat the following elements like block-level elements. It should be more clear when CSS support is better.
				CaseInsensitiveStringSet similar_to_block_level_elements({"html", "body", "td", "tr", "th"});
				if (block_level_elements.count(tag_name) || similar_to_block_level_elements.count(tag_name))
				{
					// Remove sequences of spaces at the end of an block-level element
					// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace
					boost::trim_right(m_buffered_text);
					m_last_char_in_inline_formatting_context = '\0'; // inline formatting context is now empty
				}
				m_parser->sendTag(tag::Text{.text = m_buffered_text});
				m_buffered_text = "";
			}
			// All html elements that will emit tag::Paragraph (as we do not have H1 etc)
			CaseInsensitiveStringSet paragraph_elements({"h1", "h2", "h3", "h4", "h5", "h6", "p"});
			if (isEnd)
			{
				if (strcasecmp(node.tagName().c_str(), "title") == 0)
					m_in_title = false;
				else if (strcasecmp(node.tagName().c_str(), "style") == 0)
				{
					m_in_style = false;
					parseCSS();
					m_parser->sendTag(tag::Style{.css_text = m_style_text});
					m_style_text.clear();
				}
				else if (paragraph_elements.count(tag_name))
				{
					m_parser->sendTag(tag::CloseParagraph{});
				}
				else if (str_iequals(tag_name, "div"))
				{
					m_parser->sendTag(tag::CloseSection{});
				}
				else if (str_iequals(tag_name, "span"))
				{
					m_parser->sendTag(tag::CloseSpan{});
				}
				else if (str_iequals(tag_name, "a"))
				{
					m_parser->sendTag(tag::CloseLink{});
        }
				else if (str_iequals(tag_name, "table"))
				{
					m_parser->sendTag(tag::CloseTable{});
				}
				else if (str_iequals(tag_name, "tr"))
				{
					m_parser->sendTag(tag::CloseTableRow{});
				}
				else if (str_iequals(tag_name, "td") || str_iequals(tag_name, "th"))
				{
					m_parser->sendTag(tag::CloseTableCell{});
				}
				else if ((strcasecmp(node.tagName().c_str(), "script") == 0 || strcasecmp(node.tagName().c_str(), "iframe") == 0) && m_in_script)
				{
					m_in_script = false;
				}
				else if (str_iequals(tag_name, "ul") || str_iequals(tag_name, "ol"))
				{
					m_parser->sendTag(tag::CloseList{});
				}
				else if (str_iequals(tag_name, "li"))
				{
					m_parser->sendTag(tag::CloseListItem{});
				}
				else if (str_iequals(tag_name, "b"))
				{
					m_parser->sendTag(tag::CloseBold{});
				}
				else if (str_iequals(tag_name, "u"))
				{
					m_parser->sendTag(tag::CloseUnderline{});
				}
			}
			else if (strcasecmp(node.tagName().c_str(), "title") == 0)
				m_in_title = true;
			else if (strcasecmp(node.tagName().c_str(), "style") == 0)
				m_in_style = true;
			else if (paragraph_elements.count(node.tagName()))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Paragraph{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "div"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Section{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "span"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Span{.styling=html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "ol") == 0
					 || strcasecmp(node.tagName().c_str(), "ul") == 0)
			{
				node.parseAttributes();
				bool style_type_none = false;
				std::pair<bool, std::string> style = node.attribute("style");
				std::map<std::string, std::string> style_attributes;
				if (style.first)
					parseStyleAttributes(style.second, style_attributes);
				if (style_attributes.find("list-style") == style_attributes.end())
				{
					if (str_iequals(tag_name, "ol") && m_turn_off_ol_enumeration)
						style_type_none = true;
					else if (str_iequals(tag_name, "ul") && m_turn_off_ul_enumeration)
						style_type_none = true;
					else if (style_attributes["list-style"] == "none")
						style_type_none = true;
				}
				m_parser->sendTag(tag::List{.type = std::string(style_type_none ? "none": (str_iequals(tag_name, "ol") ? "decimal" : "disc")), .styling=html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "br") == 0)
			{
				node.parseAttributes();
				m_last_char_in_inline_formatting_context = '\0';
				m_parser->sendTag(tag::BreakLine{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "li"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::ListItem{.styling=html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "script") == 0 || strcasecmp(node.tagName().c_str(), "iframe") == 0)
			{
				m_in_script = true;
			}
			else if (strcasecmp(node.tagName().c_str(), "a") == 0)
			{
				node.parseAttributes();
				std::string url;
				std::pair<bool, std::string> href = node.attribute("href");
				if (href.first && href.second.length() > 0)
				{
					url = href.second;
					//skip target if begins with "#" or "javascript"
					if (url[0] == '#' || url.find("javascript") == 0)
						url = "";
					else
						convertToUtf8(url);
				}
				m_parser->sendTag(tag::Link{.url = url, .styling = html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "img") == 0)
			{
				node.parseAttributes();
				std::string src;
				std::pair<bool, std::string> src_attr = node.attribute("src");
				if (src_attr.first)
				{
					src = src_attr.second;
					convertToUtf8(src);
				}
				std::string alt;
				std::pair<bool, std::string> alt_attr = node.attribute("alt");
				if (alt_attr.first)
				{
					alt = alt_attr.second;
					convertToUtf8(alt);
				}
				m_parser->sendTag(tag::Image{.src = src, .alt = alt, .styling = html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "table") == 0)
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Table{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "tr"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::TableRow{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "td") || str_iequals(tag_name, "th"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::TableCell{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "b"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Bold{.styling=html_node_styling(node)});
			}
			else if (str_iequals(tag_name, "u"))
			{
				node.parseAttributes();
				m_parser->sendTag(tag::Underline{.styling=html_node_styling(node)});
			}
			else if (strcasecmp(node.tagName().c_str(), "body") == 0)
			{
				//if we still don't have an encoding, we can try guess it.
				if (m_charset.empty())
				{
					csd_t charset_detector = csd_open();
					if (charset_detector == (csd_t)-1)
					{
						m_parser->sendTag(make_error_ptr("Could not create charset detector"));
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
							docwire_log(debug) << "Could not found explicit information about encoding. Estimated encoding: " + m_charset;
							createCharsetConverter();
						}
					}
					//if we still don't know which encoding is used...
					if (m_charset.empty())
					{
						docwire_log(debug) << "Could not detect encoding. Document is assumed to be encoded in UTF-8";
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
						docwire_log(debug) << "Following encoding was detected: " + m_charset;
						createCharsetConverter();
					}
				}
			}
		}

		void foundText(Node node) override
		{
			std::string text = node.text();
			docwire_log(debug) << "HTML text found: [" << text << "]";
			if (m_in_style)
			{
				std::string text = node.text();
				text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
				m_style_text += text;
				return;
			}
			if (m_in_title || m_in_script)
				return;
			// https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Whitespace#what_is_whitespace
			// Convert all whitespaces into spaces and reduce all adjacent spaces into a single space
			{
				static std::mutex whitespaces_regex_mutex;
				std::lock_guard<std::mutex> whitespaces_regex_mutex_lock(whitespaces_regex_mutex);
				static const std::regex whitespaces_regex(R"(\s+)");
				text = std::regex_replace(text, whitespaces_regex, " ");
			}
			docwire_log(debug) << "After converting and reducing whitespaces: [" << text << "]";
			bool last_char_was_space = isspace((unsigned char)m_last_char_in_inline_formatting_context);
			docwire_log(debug) << "Last char in inline formatting context was whitespace: " << last_char_was_space;
			// Reduce whitespaces between text nodes (end of previous and beginning of current.
			// Remove whitespaces from beginning of inline formatting context.
			if (last_char_was_space || m_last_char_in_inline_formatting_context == '\0')
			{
				boost::trim_left(text);
				docwire_log(debug) << "After reducing whitespaces between text nodes and removing whitespaces from begining of inline formatting context: [" << text << "]";
			}
			convertToUtf8(text);
			docwire_log(debug) << "After converting to utf8: [" << text << "]";
			if (!text.empty())
			{
				m_last_char_in_inline_formatting_context = text.back();
				// buffer text because whitespaces from end of inline formatting context should be removed.
				m_buffered_text += text;
			}
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
						docwire_log(debug) << "Following encoding was detected: " + m_charset;
						createCharsetConverter();
					}
				}
			}
		}

	public:
		SaxParser(std::string& html_content, bool skip_decoding, const HTMLParser* parser)
			: m_in_title(false), m_in_style(false), m_converter(nullptr), m_decoded_buffer(nullptr),
			m_in_script(false), m_decoded_buffer_size(0),
			  m_turn_off_ul_enumeration(false), m_turn_off_ol_enumeration(false),
			  m_html_content(html_content), m_skip_decoding(skip_decoding), m_parser(parser), m_last_char_in_inline_formatting_context('\0')
		{
		}

		~SaxParser() override
		{
			if (m_decoded_buffer)
				delete[] m_decoded_buffer;
			if (m_converter)
				delete m_converter;
		}
};

class MetaSaxParser : public ParserSax
{
	private:
		attributes::Metadata& m_meta;

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
					m_meta.author = content.second;
				else if (strcasecmp(name.second.c_str(), "changedby") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.contributor") == 0)
				{
					// Multiple changedby meta tags are possible - LibreOffice 3.5 is an example
					if (!m_meta.last_modified_by)
						m_meta.last_modified_by = content.second;
				}
				else if (strcasecmp(name.second.c_str(), "created") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.issued") == 0)
				{
					tm creation_date;
					if (string_to_date(content.second, creation_date))
					{
					  m_meta.creation_date = creation_date;
					}
				}
				else if (strcasecmp(name.second.c_str(), "changed") == 0 ||
						strcasecmp(name.second.c_str(), "dcterms.modified") == 0)
				{
					// Multiple changed meta tags are possible - LibreOffice 3.5 is an example
					if (!m_meta.last_modification_date)
					{
						tm last_modification_date;
						if (string_to_date(content.second, last_modification_date))
						{
							m_meta.last_modification_date = last_modification_date;
						}
					}
				}
			}
		}

	public:
		MetaSaxParser(attributes::Metadata& meta)
			: m_meta(meta)
		{
		};
};

struct HTMLParser::Implementation
{
	bool m_skip_decoding = false;
};

HTMLParser::HTMLParser()
	: impl(std::make_unique<Implementation>())
{
}

HTMLParser::~HTMLParser() = default;

Parser&
HTMLParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
	return *this;
}

bool HTMLParser::understands(const data_source& data) const
{
	docwire_log_func();
	std::string initial_xml = data.string(length_limit{1024});
	if (initial_xml.find("<html") != std::string::npos || initial_xml.find("<HTML") != std::string::npos)
		return true;
	return false;
}

void
HTMLParser::parse(const data_source& data) const
{
	docwire_log(debug) << "Using HTML parser.";
	std::string content = data.string();
	sendTag(tag::Document
		{
			.metadata = [this, &content]()
			{
				docwire_log(debug) << "Extracting metadata.";
				attributes::Metadata meta;
				MetaSaxParser parser(meta);
				parser.parse(content);
				return meta;				
			}
		});
	SaxParser parser(content, impl->m_skip_decoding, this);
	parser.parse(content);
	sendTag(tag::CloseDocument{});
}

void HTMLParser::skipCharsetDecoding()
{
	impl->m_skip_decoding = true;
}

} // namespace docwire
