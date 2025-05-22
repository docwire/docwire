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

#include "iwork_parser.h"

#include "data_source.h"
#include "error_tags.h"
#include <stack>
#include <stdlib.h>
#include <iostream>
#include "log.h"
#include <stdio.h>
#include <sstream>
#include "scoped_stack_push.h"
#include "tags.h"
#include "throw_if.h"
#include "zip_reader.h"
#include "entities.h"
#include <map>
#include "make_error.h"
#include <list>
#include <vector>
#include <cmath>
#include "misc.h"

namespace docwire
{

//Functions for parsing date/duration. Not sure if they should be here (in this file)
static std::string ParseDate(std::string& format, long value)
{
	//by default value "0" is "1st january 2001 midnight". At least in iWorks
	std::ostringstream os;
	// warning TODO: mktime is not working for some reason... I will use my own function.
	int months_length[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int hour = 0;
	int minute = 0;
	int second = 0;
	int day = 0;
	int month = 0;
	int year = 2001;
	int year_in_seconds = 365 * 24 * 3600;
	while (value < 0)
	{
		year -= 1;
		if (((year % 100) != 0 && (year % 4) == 0) || ((year % 100) == 0 && ((year / 100) % 4) == 0))
		{
			year_in_seconds = 366 * 24 * 3600;
			months_length[1] = 29;
		}
		else
		{
			year_in_seconds = 365 * 24 * 3600;
			months_length[1] = 28;
		}
		value += year_in_seconds;
	}
	while (value >= year_in_seconds)
	{
		year += 1;
		value -= year_in_seconds;
		if (((year % 100) != 0 && (year % 4) == 0) || ((year % 100) == 0 && ((year / 100) % 4) == 0))
		{
			year_in_seconds = 366 * 24 * 3600;
			months_length[1] = 29;
		}
		else
		{
			year_in_seconds = 365 * 24 * 3600;
			months_length[1] = 28;
		}
	}
	int month_in_seconds;
	month_in_seconds = months_length[month] * 24 * 3600;
	while (value >= month_in_seconds)
	{
		month += 1;
		value -= month_in_seconds;
		month_in_seconds = months_length[month] * 24 * 3600;
	}
	while (value >= 24 * 3600)
	{
		day += 1;
		value -= 24 * 3600;
	}
	while (value >= 3600)
	{
		hour += 1;
		value -= 3600;
	}
	while (value >= 60)
	{
		minute += 1;
		value -= 60;
	}
	second = value;
	std::string am_pm = "AM";
	if (hour >= 12)	//correct, we are using 12-hour clock in iWorks. Or am I missing something?
	{
		am_pm = "PM";
		hour -= 12;
	}
	if (hour == 0)
		hour = 12;
	std::string element;
	for (int i = 0; i < format.length(); ++i)
	{
		bool finished_element = false;
		char c = format[i];

		if (c == 'd' || c == 'M' || c == 'y' || c == 'Y' || c == 'h' || c == 'm' || c == 's' || c == 'a')
			element += c;
		else
			finished_element = true;

		if (i == format.length() - 1 || finished_element)
		{
			if (element == "d")
				os << (day + 1);
			else if (element == "dd")
			{
				if (day + 1 < 10)
					os << 0;
				os << (day + 1);
			}
			else if (element == "M")
				os << (month + 1);
			else if (element == "MM")
			{
				if (month + 1 < 10)
					os << 0;
				os << (month + 1);
			}
			else if (element == "yy" || element == "YY")
			{
				if (year % 100 < 10)
					os << 0;
				os << (year % 100);
			}
			else if (element == "yyyy" || element == "YYYY")
				os << year;
			else if (element == "h")
				os << hour;
			else if (element == "hh")
			{
				if (hour < 10)
					os << 0;
				os << hour;
			}
			else if (element == "m")
				os << minute;
			else if (element == "mm")
			{
				if (minute < 10)
					os << 0;
				os << minute;
			}
			else if (element == "s")
				os << second;
			else if (element == "ss")
			{
				if (second < 10)
					os << 0;
				os << second;
			}
			else if (element == "a")
				os << am_pm;
			element.clear();
		}
		if (finished_element)
			os << format[i];
	}
	return os.str();
}

static std::string ParseDuration(std::string& format, long value)
{
	std::ostringstream os;
	std::string element;
	for (int i = 0; i < format.length(); ++i)
	{
		bool finished_element = false;
		char c = format[i];

		if (c == 'h' || c == 'm' || c == 's')
			element += c;
		else
			finished_element = true;

		if (i == format.length() - 1 || finished_element)
		{
			if (element == "hhh" || element == "hhhh")  //hour?
			{
				int h = value / 3600;
				os << h;
				os << "h";
				value -= h * 3600;
			}
			else if (element == "mmm" || element == "mmmm") //minute?
			{
				int m = value / 60;
				os << m;
				os << "m";
				value -= m * 60;
			}
			else if (element == "sss" || element == "ssss") //second?
			{
				int s = value;
				os << s;
				os << "s";
				value -= s;
			}
			element.clear();
		}
		if (finished_element)
			os << format[i];
	}
	return os.str();
}

static std::string find_main_xml_file(const ZipReader& unzip)
{
	if (unzip.exists("index.xml"))
		return "index.xml";
	if (unzip.exists("index.apxl"))
		return "index.apxl";
	if (unzip.exists("presentation.apxl"))
		return "presentation.apxl";
	throw make_error("None of the following files (index.xml, index.apxl, presentation.apxl) could be found.", errors::uninterpretable_data{});
}

namespace
{

struct context
{
	const emission_callbacks& emit_tag;
	std::string m_xml_file;
};

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/vnd.apple.pages"},
	mime_type{"application/vnd.apple.numbers"},
	mime_type{"application/vnd.apple.keynote"},
	mime_type{"application/x-iwork-pages-sffpages"},
	mime_type{"application/x-iwork-numbers-sffnumbers"},
	mime_type{"application/x-iwork-keynote-sffkey"}
};

} // anonymous namespace

template<>
struct pimpl_impl<IWorkParser> : pimpl_impl_base
{
	void parse(const data_source& data, const emission_callbacks& emit_tag);
	std::stack<context> m_context_stack;

	continuation emit_tag(Tag&& tag) const
	{
		return m_context_stack.top().emit_tag(std::move(tag));
	}

	class DataSource
	{
		private:
			ZipReader* m_zipfile;
			bool m_done;
			std::string m_xml_file;

		public:
			DataSource(ZipReader& zipfile, std::string& xml_file)
			{
				m_zipfile = &zipfile;
				m_done = false;
				m_xml_file = xml_file;
			}

			void ReadChunk(char* chunk, int len, bool error_if_no_data, int& readed)
			try
			{
				chunk[0] = '\0';
				if (m_done)
				{
					readed = 0;
					return;
				}
				throw_if (!m_zipfile->readChunk(m_xml_file, chunk, len, readed), m_xml_file, len);
				if (readed != len)	//we have reached the end
					m_done = true;
				throw_if (error_if_no_data && readed == 0, "Unexpected end of file", errors::uninterpretable_data{});
			}
			catch (const std::exception&)
			{
				std::throw_with_nested(make_error(m_xml_file));
			}
	};

	class XmlReader
	{
		public:
			enum XmlElementKind
			{
				opening_element,	//<something>
				closing_element,	//</something>
				empty_element,		//<something/>
				characters,			//it's not xml element but characters beetween > and <
				no_element			//end of data
			};

			struct XmlElement
			{
				XmlElementKind m_kind;
				std::map<std::string, std::string> m_attributes;
				std::string m_name;
				std::string m_characters;

				XmlElement()
				{
					m_name.reserve(50);
					m_characters.reserve(1024);
					Clear();
				}

				void Clear()
				{
					m_name.clear();
					m_characters.clear();
					m_kind = opening_element;
					m_attributes.clear();
				}

				bool HasAttribute(const std::string& attribute_name)
				{
					return m_attributes.find(attribute_name) != m_attributes.end();
				}
			};

		private:
			bool m_tag_opened;
			DataSource* m_data_source;
			int m_chunk_len;
			char m_chunk[1025];
			int m_pointer;

		public:
			//SkipAttributes is faster than ReadXmlAttributes. If I dont need to know attributes,
			//I should call this method. It speeds up parsing file

			void SkipAttributes()
			{
				if (!m_tag_opened)
					return;
				while (true)
				{
					for (; m_pointer < m_chunk_len; ++m_pointer)
					{
						if (m_chunk[m_pointer] == '>')
						{
							m_tag_opened = false;
							++m_pointer;
							return;
						}
					}
					m_data_source->ReadChunk(m_chunk, 1024, true, m_chunk_len);
					m_pointer = 0;
				}
			}

			void ReadXmlAttributes(XmlElement& xml_element)
			{
				if (!m_tag_opened)
					return;
				bool reading_attribute_name = true;
				bool reading_attribute_value = false;
				std::string attribute_name = "";
				std::string* attribute_value;
				attribute_name.reserve(50);
				while (true)
				{
					for (; m_pointer < m_chunk_len; ++m_pointer)
					{
						if (m_chunk[m_pointer] == '>')
						{
							m_tag_opened = false;
							++m_pointer;
							return;
						}
						else if (reading_attribute_value == false && m_chunk[m_pointer] == '/')
						{
							xml_element.m_kind = empty_element;
						}
						else if (reading_attribute_name && m_chunk[m_pointer] != ' ')
						{
							if (m_chunk[m_pointer] != '=')
								attribute_name += m_chunk[m_pointer];
							else
							{
								reading_attribute_name = false;
								attribute_value = &xml_element.m_attributes[attribute_name];
								attribute_value->reserve(50);
								attribute_name.clear();
							}
						}
						else
						{
							if (!reading_attribute_value)
							{
								if (m_chunk[m_pointer] == '"')
									reading_attribute_value = true;
							}
							else
							{
								if (m_chunk[m_pointer] != '"')
									*attribute_value += m_chunk[m_pointer];
								else
								{
									reading_attribute_value = false;
									reading_attribute_name = true;
								}
							}
						}
					}
					m_data_source->ReadChunk(m_chunk, 1024, true, m_chunk_len);
					m_pointer = 0;
				}
			}

			XmlReader(DataSource& data_source)
			{
				m_data_source = &data_source;
				m_tag_opened = false;
				m_pointer = 0;
				m_chunk_len = 0;
				m_chunk[0] = '\0';
			}

			void GetNextElement(XmlElement& xml_element)
			{
				xml_element.Clear();
				char c;
				while (true)
				{
					for (; m_pointer < m_chunk_len; ++m_pointer)
					{
						c = m_chunk[m_pointer];
						if (c == '<')
						{
							if (xml_element.m_characters.length() > 0)
							{
								xml_element.m_kind = characters;
								return;
							}
							m_tag_opened = true;
						}
						else if (c == '>')
						{
							m_tag_opened = false;
							++m_pointer;
							return;
						}
						else if (m_tag_opened == false) //some character beetwen > and <
						{
							xml_element.m_characters += c;
						}
						else if (c > 32 && c != '/')	//some character beetwen < and > (but not '/' or space)
						{
							xml_element.m_name += c;
						}
						else if (c == '/')
						{
							if (xml_element.m_name.length() == 0)	  //</something>
								xml_element.m_kind = closing_element;
							else										//<something/>
								xml_element.m_kind = empty_element;
						}
						else
						{
							//this is an element with attributes: <something [attributes].
							//For now I'm not going to read them (attributes)
							++m_pointer;
							return;
						}
					}
					m_data_source->ReadChunk(m_chunk, 1024, false, m_chunk_len);
					m_pointer = 0;
					if (m_chunk_len == 0)	//end of data
					{
						xml_element.m_kind = no_element;
						return;
					}
				}
			}
	};

	struct IWorkContent
	{
		enum IWorkType
		{
			pages,
			numbers,
			key,
			encrypted
		};

		//each xml element which I am interested in will be handled by own function
		typedef void (IWorkContent::*XmlElementHandler)(void);

		struct NumberFormat
		{
			int m_type_numbers;			//money=1, normal number=0, percentage=2
			std::string m_format_string;	//I should add more support for this parameter
			int m_fraction_precision;
			std::string m_currency;

			NumberFormat()
			{
				m_type_numbers = 0;
				m_fraction_precision = 0;
			}
		};

		struct CellStyle
		{
			NumberFormat* m_number_format;
			std::string m_date_format;
			std::string m_duration_format;
			std::string m_id;

			CellStyle()
			{
				m_number_format = NULL;
				m_date_format = "dd.MM.yyyy hh:mm a";
				m_duration_format = "hhh mmm sss";
			}
		};

		struct TableCell
		{
			bool m_is_date;
			bool m_is_duration;
			bool m_is_text;
			bool m_is_number;
			CellStyle* m_cell_style;
			std::string m_value;

			TableCell()
			{
				m_value.reserve(2048);
				Clear();
			}

			void Clear()
			{
				m_value.clear();
				m_is_duration = false;
				m_is_date = false;
				m_is_text = false;
				m_is_number = true;
				m_cell_style = NULL;
			}
		};

		struct TextualElement
		{
			double m_x, m_y;
			std::string m_text;
			std::string* m_text_pointer;

			TextualElement()
			{
				m_x = 0;
				m_y = 0;
				m_text.reserve(4096);
				m_text_pointer = &m_text;
			}

			virtual ~TextualElement()
			{
			}
		};

		struct Table : public TextualElement
		{
			int m_rows_count;
			int m_columns_count;
			int m_current_column;
			bool m_in_pm;
			TableCell m_current_cell;
			std::map<std::string, std::string> m_pms;
			std::string m_selected_pm;
			std::list<int> m_cells_per_rows;

			Table()
			{
				m_rows_count = 0;
				m_columns_count = 0;
				m_current_column = 0;
				m_in_pm = false;
				m_text.reserve(20480);
			}

			void AddCell()
			{
				CellStyle* cell_style = m_current_cell.m_cell_style;
				NumberFormat* number_format = cell_style == NULL ? NULL : cell_style->m_number_format;
				std::ostringstream os;

				if (m_current_cell.m_is_duration)
				{
					std::string duration_format = "hhh mmm sss";
					if (cell_style != NULL && cell_style->m_duration_format.length() > 0)
						duration_format = cell_style->m_duration_format;
					long value = strtol(m_current_cell.m_value.c_str(), NULL, 0);
					m_current_cell.m_value = ParseDuration(duration_format, value);
				}
				else if (m_current_cell.m_is_date)
				{
					std::string date_format = "dd.MM.yyyy hh:mm a";
					if (cell_style != NULL && cell_style->m_date_format.length() > 0)
						date_format = cell_style->m_date_format;
					long value = strtol(m_current_cell.m_value.c_str(), NULL, 0);
					m_current_cell.m_value = ParseDate(date_format, value);
				}
				else if (m_current_cell.m_is_number)
				{
					double value = strtod(m_current_cell.m_value.c_str(), NULL);
					if (number_format == NULL)
						os << std::fixed << (long)value;
					else
					{
						//format string looks like: ¤#,##0.00;-¤#,##0.00
						//after ';' we have style for negative number. If '-' is missing, we should get absolute value
						if (value < 0.0 && number_format->m_format_string.find(";-") == std::string::npos)
							value = -value;
						switch (number_format->m_type_numbers)
						{
							default:	//others
							case 0:	 //number
							{
								if (number_format->m_fraction_precision > 0)
								{
									os.precision(number_format->m_fraction_precision);
									os << std::fixed << value;
								}
								else
									os << std::fixed << (long)round(value);
								break;
							}
							case 1:	//money
							{
								if (number_format->m_currency == "EUR")
									os << "€";
								else if (number_format->m_currency == "USD" || number_format->m_currency == "MXN")
									os << "$";
								if (number_format->m_fraction_precision > 0)
								{
									os.precision(number_format->m_fraction_precision);
									os << std::fixed << value;
								}
								else
									os << std::fixed << (long)round(value);
								break;
							}
							case 2:	//percentage
							{
								value *= 100;
								if (number_format->m_fraction_precision > 0)
								{
									os.precision(number_format->m_fraction_precision);
									os << std::fixed << value;
								}
								else
									os << std::fixed << (long)round(value);
								os << "%";
								break;
							}
						}
						m_current_cell.m_value = os.str();
					}
				}
				if (!m_in_pm)
					*m_text_pointer += m_current_cell.m_value;
				else
					m_pms[m_selected_pm] += m_current_cell.m_value;
				m_current_cell.Clear();
			}

			void FinishCell()
			{
				bool new_row = false;
				++m_current_column;
				if (m_cells_per_rows.size() > 0)
				{
					if (m_cells_per_rows.front() == m_current_column)
					{
						m_cells_per_rows.pop_front();
						new_row = true;
					}
				}
				else if (m_current_column == m_columns_count)
					new_row = true;
				if (new_row)
				{
					for (size_t i = m_current_column; i < m_columns_count; ++i)
						*m_text_pointer += "	";
					*m_text_pointer += "\n";
					m_current_column = 0;
				}
				else
					*m_text_pointer += "	";
			}
		};

		struct Chart : public TextualElement
		{
			int m_series_count;
			int m_chart_direction;
			bool m_filling_columns;
			bool m_filling_rows;
			std::list<std::string> m_row_names;
			std::list<std::string> m_column_names;

			Chart()
			{
				m_series_count = -1;
				m_chart_direction = 0;
				m_filling_columns = false;
				m_filling_rows = false;
			}

			void FinishChart()
			{
				bool print_rows = false;
				bool print_columns = false;
				std::list<std::string>::iterator it;

				if (m_chart_direction == 1)
					print_columns = true;
				else if (m_chart_direction == 0)
					print_rows = true;

				if (print_columns)
				{
					it = m_column_names.begin();
					int i = 0;
					while (it != m_column_names.end())
					{
						if (m_series_count != -1 && i >= m_series_count)
							break;
						m_text += (*it) + "\n";
						++it;
						++i;
					}
				}
				if (print_rows)
				{
					it = m_row_names.begin();
					int i = 0;
					while (it != m_row_names.end())
					{
						if (m_series_count != -1 && i >= m_series_count)
							break;
						m_text += (*it) + "\n";
						++it;
						++i;
					}
				}
				m_row_names.clear();
				m_column_names.clear();
			}
		};

		//a lot of variables to store data. Some of them are common beetwen pages/numbers/key, some are special
		std::string* m_current_text_pointer;
		std::string m_text_body;
		XmlReader::XmlElement m_current_element;
		bool m_reading_text;
		int m_pages_count;
		std::list<TextualElement*> m_textual_elements;
		std::map<std::string, CellStyle> m_cell_styles;
		std::map<std::string, NumberFormat> m_number_formats;
		std::map<std::string, std::string> m_date_formats;
		std::map<std::string, std::string> m_duration_formats;
		CellStyle* m_current_cell_style;
		Table* m_current_table;
		Chart* m_current_chart;
		TextualElement* m_current_textual;
		bool m_in_textual_element;
		bool m_in_table;
		bool m_in_cell_style;
		bool m_in_chart;
		bool m_in_formula;
		std::string m_notes;
		std::vector<double> m_offsets;
		bool m_in_slide;
		bool m_in_notes;
		bool m_next_position_for_group;
		bool m_in_title;
		bool m_in_annotation;
		std::map<std::string, std::string> m_annotations;
		std::map<std::string, std::string> m_attachments;
		std::list<std::string> m_prev_attachments;
		std::list<std::string> m_footnotes;
		std::string m_tmp_footer;
		std::string m_footer;
		std::string m_tmp_header;
		std::string m_header;
		std::string m_attachment_name;
		bool m_inside_footers;
		bool m_inside_footnotes;
		bool m_inside_attachment;
		bool m_inside_headers;
		bool m_inside_text_body;
		int m_footnote_start;
		int m_footnote_count;
		IWorkType m_iwork_type;
		XmlReader* m_xml_reader;
		std::map<std::string, XmlElementHandler> m_handlers;

		IWorkContent(XmlReader& xml_reader)
		{
			m_xml_reader = &xml_reader;
			m_in_annotation = false;
			m_text_body.reserve(1024 * 1024);
			m_notes.reserve(20480);
			m_footer.reserve(2048);
			m_header.reserve(2048);
			m_tmp_header.reserve(2048);
			m_tmp_footer.reserve(2048);
			m_footnote_start = 0;
			m_footnote_count = 0;
			m_inside_footers = false;
			m_inside_headers = false;
			m_inside_text_body = false;
			m_inside_attachment = false;
			m_inside_footnotes = false;
			m_current_text_pointer = &m_text_body;
			m_reading_text = false;
			m_pages_count = 0;
			m_next_position_for_group = false;
			m_in_title = false;
			m_in_notes = false;
			m_in_slide = false;
			m_current_table = NULL;
			m_current_cell_style = NULL;
			m_current_textual = NULL;
			m_current_chart = NULL;
			m_in_textual_element = false;
			m_in_table = false;
			m_in_cell_style = false;
			m_in_chart = false;
			m_in_formula = false;
			RegisterHandlers();
		}

		~IWorkContent()
		{
			ClearTextualElements();
		}

		void setType(IWorkType iwork_type)
		{
			m_iwork_type = iwork_type;
		}

		void ClearTextualElements()
		{
			std::list<TextualElement*>::iterator it = m_textual_elements.begin();
			while (it != m_textual_elements.end())
			{
				delete (*it);
				++it;
			}
			m_textual_elements.clear();
		}

		static bool CompareElements(TextualElement* first, TextualElement* second)
		{
			if (first->m_y < second->m_y)
				return true;
			if (first->m_y == second->m_y && first->m_x < second->m_x)
				return true;
			return false;
		}

		void AddTextFromTextualElements()
		{
			m_textual_elements.sort(CompareElements);
			std::list<TextualElement*>::iterator it = m_textual_elements.begin();
			while (it != m_textual_elements.end())
			{
				m_text_body += (*it)->m_text;
				++it;
			}
		}

		//Functions responsible for parsing xml elements:

		void ParseLsWorkspace()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				++m_pages_count;
				if (m_pages_count > 1)
					(*m_current_text_pointer) += "\n";
			}
			else if (m_current_element.m_kind == XmlReader::closing_element)
			{
				AddTextFromTextualElements();
				ClearTextualElements();
			}
		}

		void ParseSfP()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
				m_reading_text = true;
			else
				m_reading_text = false;
		}

		void ParseSfBr()
		{
			(*m_current_text_pointer) += "\n";
		}

		void ParseSfTab()
		{
			(*m_current_text_pointer) += "	";
		}

		void ParseSfTabularInfo()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				Table* table = NULL;
				try
				{
					table = new Table();
					m_current_table = table;
					m_current_textual = table;
					m_in_textual_element = true;
					if (m_iwork_type == pages)
						m_current_table->m_text_pointer = m_current_text_pointer;
					else
						*(m_current_table->m_text_pointer) += "\n";
					m_current_text_pointer = &m_current_table->m_current_cell.m_value;
					m_in_table = true;
					m_textual_elements.push_back(table);
				}
				catch (std::bad_alloc& ba)
				{
					if (table)
						delete table;
					throw;
				}
			}
			else
			{
				if (m_iwork_type == pages)
					m_current_text_pointer = m_current_table->m_text_pointer;
				else
					m_current_text_pointer = &m_text_body;
				m_in_table = false;
				m_in_textual_element = false;
				m_current_table = NULL;
				m_current_textual = NULL;
			}
		}

		void ParseSfTabularModel()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sf:name") && m_current_element.HasAttribute("sf:name-is-visible"))
				{
					if (m_current_element.m_attributes["sf:name-is-visible"] == "true")
						*(m_current_table->m_text_pointer) += (m_current_element.m_attributes["sf:name"] + "\n");
				}
			}
		}

		void ParseSfPosition()
		{
			if (!m_current_element.HasAttribute("sfa:x") || !m_current_element.HasAttribute("sfa:y"))
				return;
			double x = strtod(m_current_element.m_attributes["sfa:x"].c_str(), NULL);
			double y = strtod(m_current_element.m_attributes["sfa:y"].c_str(), NULL);
			if (m_next_position_for_group)
			{
				m_next_position_for_group = false;
				m_offsets.push_back(x);
				m_offsets.push_back(y);
			}
			else if (m_in_textual_element)
			{
				m_current_textual->m_x = x;
				m_current_textual->m_y = y;
				for (size_t i = 0; i < m_offsets.size(); ++i)	//position may be relative
				{
					if (i % 2 == 0)
						m_current_textual->m_x += m_offsets[i];
					else
						m_current_textual->m_y += m_offsets[i];
				}
			}
		}

		void ParseSfRows()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sf:count"))
					m_current_table->m_rows_count = strtol(m_current_element.m_attributes["sf:count"].c_str(), NULL, 0);
			}
		}

		void ParseSfColumns()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sf:count"))
					m_current_table->m_columns_count = strtol(m_current_element.m_attributes["sf:count"].c_str(), NULL, 0);
			}
		}

		void ParseSfGridRow()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sf:nc"))
					m_current_table->m_cells_per_rows.push_back(strtol(m_current_element.m_attributes["sf:nc"].c_str(), NULL, 0));
			}
		}

		void ParseSfPm()
		{
			if (m_in_table)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_current_table->m_in_pm = true;
				else
				{
					m_current_table->m_in_pm = false;
					std::string selected_id = m_current_table->m_selected_pm;
					m_current_text_pointer = &m_current_table->m_current_cell.m_value;
					if (m_current_table->m_pms.find(selected_id) != m_current_table->m_pms.end())
						(*m_current_text_pointer) += m_current_table->m_pms[selected_id];
					m_current_table->m_current_cell.m_is_number = false;
					m_current_table->m_current_cell.m_is_text = true;
					m_current_table->AddCell();
					m_current_table->FinishCell();
					m_current_table->m_pms.clear();
				}
			}
		}

		void ParseSfT()
		{
			if (m_in_table)
			{
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					m_current_table->AddCell();
					if (m_current_table->m_in_pm == false)
						m_current_table->FinishCell();
				}
				else
				{
					m_current_text_pointer = &m_current_table->m_current_cell.m_value;
					m_current_table->m_current_cell.m_is_text = true;
					m_current_table->m_current_cell.m_is_number = false;
					if (m_current_table->m_in_pm && m_current_element.HasAttribute("sfa:ID"))
						m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:ID"];
				}
			}
		}

		void ParseSfN()
		{
			if (m_in_table)
			{
				if (m_current_table->m_in_pm)
				{
					if (m_current_element.HasAttribute("sfa:ID"))
						m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:ID"];
				}
				if (m_current_element.HasAttribute("sf:v"))
					(*m_current_text_pointer) = m_current_element.m_attributes["sf:v"];
				if (m_current_element.HasAttribute("sf:s"))
				{
					std::string format_name = m_current_element.m_attributes["sf:s"];
					if (m_cell_styles.find(format_name) != m_cell_styles.end())
						m_current_table->m_current_cell.m_cell_style = &m_cell_styles[format_name];
				}
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					m_current_table->AddCell();
					if (m_current_table->m_in_pm == false)
						m_current_table->FinishCell();
				}
			}
		}

		void ParseSfS()
		{
			if (m_current_element.m_kind != XmlReader::opening_element)
			{
				if (m_in_table && m_current_table->m_in_pm == false)
					m_current_table->FinishCell();
			}
		}

		void ParseSfRd()
		{
			if (m_current_element.HasAttribute("sf:cell-date") && m_in_table)
			{
				*m_current_text_pointer = m_current_element.m_attributes["sf:cell-date"];
				m_current_table->m_current_cell.m_is_date = true;
				m_current_table->m_current_cell.m_is_number = false;
			}
		}

		void ParseSfD()
		{
			if (m_in_table)
			{
				if (m_current_table->m_in_pm)
				{
					if (m_current_element.HasAttribute("sfa:ID"))
						m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:ID"];
				}
				if (m_current_element.HasAttribute("sf:cell-date"))
					*m_current_text_pointer = m_current_element.m_attributes["sf:cell-date"];
				if (m_current_element.HasAttribute("sf:s"))
				{
					std::string format_name = m_current_element.m_attributes["sf:s"];
					if (m_cell_styles.find(format_name) != m_cell_styles.end())
						m_current_table->m_current_cell.m_cell_style = &m_cell_styles[format_name];
				}
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					m_current_table->m_current_cell.m_is_date = true;
					m_current_table->m_current_cell.m_is_number = false;
					m_current_table->AddCell();
					if (m_current_table->m_in_pm == false)
						m_current_table->FinishCell();
				}
			}
		}

		void ParseSfF()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sf:s"))
				{
					std::string format_name = m_current_element.m_attributes["sf:s"];
					if (m_cell_styles.find(format_name) != m_cell_styles.end())
						m_current_table->m_current_cell.m_cell_style = &m_cell_styles[format_name];
				}
				if (m_current_table->m_in_pm)
				{
					if (m_current_element.HasAttribute("sfa:ID"))
						m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:ID"];
				}
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					m_current_table->AddCell();
					if (m_current_table->m_in_pm == false)
						m_current_table->FinishCell();
					m_in_formula = false;
				}
				else
					m_in_formula = true;
			}
		}

		void ParseSfProxiedCellRef()
		{
			if (m_in_table && m_current_table->m_in_pm && m_current_element.HasAttribute("sfa:IDREF"))
				m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:IDREF"];
		}

		void ParseSfRdu()
		{
			if (m_current_element.HasAttribute("sf:du") && m_in_table)
			{
				*m_current_text_pointer = m_current_element.m_attributes["sf:du"];
				m_current_table->m_current_cell.m_is_duration = true;
				m_current_table->m_current_cell.m_is_number = false;
			}
		}

		void ParseSfDu()
		{
			if (m_in_table)
			{
				if (m_current_table->m_in_pm)
				{
					if (m_current_element.HasAttribute("sfa:ID"))
						m_current_table->m_selected_pm = m_current_element.m_attributes["sfa:ID"];
				}
				if (m_current_element.HasAttribute("sf:du"))
					*m_current_text_pointer = m_current_element.m_attributes["sf:du"];
				if (m_current_element.HasAttribute("sf:s"))
				{
					std::string format_name = m_current_element.m_attributes["sf:s"];
					if (m_cell_styles.find(format_name) != m_cell_styles.end())
						m_current_table->m_current_cell.m_cell_style = &m_cell_styles[format_name];
				}
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					m_current_table->m_current_cell.m_is_duration = true;
					m_current_table->m_current_cell.m_is_number = false;
					m_current_table->AddCell();
					if (m_current_table->m_in_pm == false)
						m_current_table->FinishCell();
				}
			}
		}

		void ParseSfRn()
		{
			if (m_current_element.HasAttribute("sf:v") && m_in_table)
				*m_current_text_pointer = m_current_element.m_attributes["sf:v"];
		}

		void ParseSfStickyNote()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				m_current_textual = NULL;
				try
				{
					m_current_textual = new TextualElement();
					m_current_text_pointer = &m_current_textual->m_text;
					m_in_textual_element = true;
					m_textual_elements.push_back(m_current_textual);
				}
				catch (std::bad_alloc& ba)
				{
					if (m_current_textual)
						delete m_current_textual;
					m_current_textual = NULL;
					throw;
				}
			}
			else
			{
				(*m_current_text_pointer) += "\n";
				m_current_text_pointer = &m_text_body;
				m_current_textual = NULL;
				m_in_textual_element = false;
			}
		}

		void ParseSfChartInfo()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				Chart* chart = NULL;
				try
				{
					Chart* chart = new Chart();
					m_current_chart = chart;
					m_current_textual = chart;
					m_current_text_pointer = &m_current_chart->m_text;
					(*m_current_text_pointer) += "\n";
					m_in_chart = true;
					m_in_textual_element = true;
					m_textual_elements.push_back(chart);
				}
				catch (std::bad_alloc& ba)
				{
					if (chart)
						delete chart;
					throw;
				}
			}
			else if (m_in_chart && m_current_element.m_kind == XmlReader::closing_element)
			{
				m_current_chart->FinishChart();
				(*m_current_text_pointer) += "\n\n";
				m_current_text_pointer = &m_text_body;
				m_in_chart = false;
				m_current_chart = NULL;
				m_current_textual = NULL;
				m_in_textual_element = false;
			}
		}

		void ParseSfFormulaChartModel()
		{
			if (m_in_chart && m_current_element.HasAttribute("sf:chart-direction"))
				m_current_chart->m_chart_direction = strtol(m_current_element.m_attributes["sf:chart-direction"].c_str(), NULL, 0);
		}

		void ParseSfString()
		{
			if (m_in_chart && m_current_element.HasAttribute("sfa:string"))
			{
				if (m_current_chart->m_filling_rows)
					m_current_chart->m_row_names.push_back(m_current_element.m_attributes["sfa:string"]);
				if (m_current_chart->m_filling_columns)
					m_current_chart->m_column_names.push_back(m_current_element.m_attributes["sfa:string"]);
			}
		}

		void ParseSfCachedSeriesCount()
		{
			if (m_in_chart && m_current_element.HasAttribute("sfa:number"))
				m_current_chart->m_series_count = strtol(m_current_element.m_attributes["sfa:number"].c_str(), NULL, 0);
		}

		void ParseSfChartName()
		{
			if (m_in_chart && m_current_element.HasAttribute("sfa:string"))
				m_current_chart->m_text += m_current_element.m_attributes["sfa:string"] + "\n";
		}

		void ParseSfChartRowNames()
		{
			if (m_in_chart)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_current_chart->m_filling_rows = true;
				else
					m_current_chart->m_filling_rows = false;
			}
		}

		void ParseSfChartColumnNames()
		{
			if (m_in_chart)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_current_chart->m_filling_columns = true;
				else
					m_current_chart->m_filling_columns = false;
			}
		}

		void ParseSfCellStyle()
		{
			if (m_current_element.HasAttribute("sfa:ID"))
			{
				m_current_cell_style = &m_cell_styles[m_current_element.m_attributes["sfa:ID"]];
				m_current_cell_style->m_id = m_current_element.m_attributes["sfa:ID"];
			}
			if (m_current_cell_style && m_current_element.m_kind == XmlReader::opening_element)
				m_in_cell_style = true;
			else
			{
				m_in_cell_style = false;
				m_current_cell_style = NULL;
			}
		}

		void ParseSfDateFormatRef()
		{
			if (m_in_cell_style)
			{
				if (m_current_element.HasAttribute("sfa:IDREF"))
				{
					std::string format_id = m_current_element.m_attributes["sfa:IDREF"];
					if (m_date_formats.find(format_id) != m_date_formats.end())
						m_current_cell_style->m_date_format = m_date_formats[format_id];
				}
			}
		}

		void ParseSfDurationFormatRef()
		{
			if (m_in_cell_style)
			{
				if (m_current_element.HasAttribute("sfa:IDREF"))
				{
					std::string format_id = m_current_element.m_attributes["sfa:IDREF"];
					if (m_duration_formats.find(format_id) != m_duration_formats.end())
						m_current_cell_style->m_duration_format = m_duration_formats[format_id];
				}
			}
		}

		void ParseSfDateFormat()
		{
			std::string format;
			if (m_current_element.HasAttribute("sf:fmt"))
				format = m_current_element.m_attributes["sf:fmt"];
			if (m_current_element.HasAttribute("sfa:ID") && format.length() > 0)
				m_date_formats[m_current_element.m_attributes["sfa:ID"]] = format;
			if (m_in_cell_style)
			{
				if (format.length() > 0)
					m_current_cell_style->m_date_format = format;
			}
			else if (format.length() > 0 && m_in_table && m_current_table->m_current_cell.m_cell_style != NULL)
				m_current_table->m_current_cell.m_cell_style->m_date_format = format;
		}

		void ParseSfDurationFormat()
		{
			std::string format;
			if (m_current_element.HasAttribute("sf:fmt"))
				format = m_current_element.m_attributes["sf:fmt"];
			if (m_current_element.HasAttribute("sfa:ID") && format.length() > 0)
				m_duration_formats[m_current_element.m_attributes["sfa:ID"]] = format;
			if (m_in_cell_style)
			{
				if (format.length() > 0)
					m_current_cell_style->m_duration_format = format;
			}
			else if (format.length() > 0 && m_in_table && m_current_table->m_current_cell.m_cell_style != NULL)
				m_current_table->m_current_cell.m_cell_style->m_duration_format = format;
		}

		void ParseSfNumberFormatRef()
		{
			if (m_in_cell_style)
			{
				if (m_current_element.HasAttribute("sfa:IDREF"))
				{
					std::string format_id = m_current_element.m_attributes["sfa:IDREF"];
					if (m_number_formats.find(format_id) != m_number_formats.end())
						m_current_cell_style->m_number_format = &m_number_formats[format_id];
				}
			}
		}

		void ParseSfNumberFormat()
		{
			NumberFormat* format = NULL;
			if (m_in_cell_style && !m_current_element.HasAttribute("sfa:ID"))	//internat number format for cell style.
			{
				std::string format_id = "Internet_format_number_" + m_current_cell_style->m_id;
				format = &m_number_formats[format_id];
			}
			if (m_current_element.HasAttribute("sfa:ID"))
			{
				std::string format_id = m_current_element.m_attributes["sfa:ID"];
				format = &m_number_formats[format_id];
			}
			if (format)
			{
				if (m_in_table && m_current_table->m_current_cell.m_cell_style != NULL)
					m_current_table->m_current_cell.m_cell_style->m_number_format = format;
				else if (m_in_cell_style)
					m_current_cell_style->m_number_format = format;

				if (m_current_element.HasAttribute("sf:format-type"))
					format->m_type_numbers = strtol(m_current_element.m_attributes["sf:format-type"].c_str(), NULL, 0);
				if (m_current_element.HasAttribute("sf:format-decimal-places"))
					format->m_fraction_precision = strtol(m_current_element.m_attributes["sf:format-decimal-places"].c_str(), NULL, 0);
				if (m_current_element.HasAttribute("sf:format-currency-code"))
					format->m_currency = m_current_element.m_attributes["sf:format-currency-code"];
				if (m_current_element.HasAttribute("sf:format-string"))
					format->m_format_string = m_current_element.m_attributes["sf:format-string"];
			}
		}

		void ParseKeySlide()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				++m_pages_count;
				if (m_pages_count > 1)
					(*m_current_text_pointer) += "\n";
				m_in_slide = true;
			}
			else if (m_current_element.m_kind == XmlReader::closing_element)
			{
				m_in_slide = false;
				AddTextFromTextualElements();
				ClearTextualElements();
				if (m_notes.length() > 0)
					m_text_body += "\n" + m_notes;
				m_notes.clear();
			}
		}

		void ParseSfGroup()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
				m_next_position_for_group = true;
			else
			{
				if (m_offsets.size() > 1)
				{
					m_offsets.pop_back();	//remove y and x offsets
					m_offsets.pop_back();
				}
			}
		}

		void ParseKeyTitlePlaceholder()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				m_in_title = true;
				m_text_body += "\n";
				m_current_text_pointer = &m_text_body;
			}
			else
			{
				m_in_title = false;
				m_text_body += "\n";
			}
		}

		void ParseKeyNotes()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				m_in_notes = true;
				m_current_text_pointer = &m_notes;
			}
			else
			{
				m_in_notes = false;
				m_current_text_pointer = &m_text_body;
			}
		}

		void ParseSfTextStorage()
		{
			if (m_iwork_type == pages)
			{
				if (m_current_element.m_kind != XmlReader::opening_element)
				{
					if (m_current_element.m_kind == XmlReader::closing_element)
						m_inside_text_body = false;
				}
				else if (m_current_element.HasAttribute("sf:kind"))
				{
					if (m_current_element.m_attributes["sf:kind"] == "body")
					{
						m_inside_text_body = true;
						if (m_text_body.length() > 0)
							m_text_body += "\n";
						m_current_text_pointer = &m_text_body;
					}
				}
			}
		}

		void ParseSfAnnotation()
		{
			if (m_current_element.m_kind == XmlReader::opening_element && m_current_element.HasAttribute("sf:target"))
			{
				m_current_text_pointer = &m_annotations[m_current_element.m_attributes["sf:target"]];
				m_current_text_pointer->reserve(2048);
				m_in_annotation = true;
			}
			else
			{
				m_in_annotation = false;
				m_current_text_pointer = &m_text_body;
			}
		}

		void ParseSfAnnotationField()
		{
			if (m_current_element.HasAttribute("sfa:ID"))
			{
				std::string id = m_current_element.m_attributes["sfa:ID"];
				if (m_annotations.find(id) != m_annotations.end())
				{
					*m_current_text_pointer += m_annotations[id];
					*m_current_text_pointer += "\n";
				}
			}
		}

		void ParseSfAttachment()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
			{
				if (m_current_element.HasAttribute("sfa:ID"))
				{
					std::string attachment_name = m_current_element.m_attributes["sfa:ID"];
					m_current_text_pointer = &m_attachments[attachment_name];
					m_current_text_pointer->reserve(4096);
					if (m_inside_attachment)
						m_prev_attachments.push_back(m_attachment_name);
					m_attachment_name = attachment_name;
					m_inside_attachment = true;
				}
			}
			else if (m_inside_attachment == true)
			{
				if (m_prev_attachments.size() == 0)
				{
					m_inside_attachment = false;
					m_current_text_pointer = &m_text_body;
					m_attachment_name.clear();
				}
				else
				{
					std::string attachment_name = m_prev_attachments.back();
					m_prev_attachments.pop_back();
					m_current_text_pointer = &m_attachments[attachment_name];
					m_attachment_name = attachment_name;
				}
			}
			else
				m_current_text_pointer = &m_text_body;
		}

		void ParseSfAttachmentRef()
		{
			if (m_current_element.HasAttribute("sfa:IDREF"))
			{
				if (m_attachments.find(m_current_element.m_attributes["sfa:IDREF"]) != m_attachments.end())
					(*m_current_text_pointer) += m_attachments[m_current_element.m_attributes["sfa:IDREF"]];
			}
		}

		void ParseSfHeader()
		{
			if (m_iwork_type == pages)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
				{
					m_inside_headers = true;
					m_tmp_header.clear();
					m_current_text_pointer = &m_tmp_header;
				}
				else
				{
					m_inside_headers = false;
					if (m_tmp_header.length() > m_header.length())
						m_header = m_tmp_header;
				}
			}
		}

		void ParseSfFooter()
		{
			if (m_iwork_type == pages)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
				{
					m_inside_footers = true;
					m_tmp_footer.clear();
					m_current_text_pointer = &m_tmp_footer;
				}
				else
				{
					m_inside_footers = false;
					if (m_tmp_footer.length() > m_footer.length())
						m_footer = m_tmp_footer;
				}
			}
		}

		void ParseSfFootnotes()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
				m_inside_footnotes = true;
			else
			{
				m_inside_footnotes = false;
				m_current_text_pointer = &m_text_body;
			}
		}

		void ParseSfCT()
		{
			if (m_in_table)
			{
				if (m_current_element.HasAttribute("sfa:s"))
					(*m_current_text_pointer) += m_current_element.m_attributes["sfa:s"];
				if (m_in_formula)
				{
					m_current_table->m_current_cell.m_is_text = true;
					m_current_table->m_current_cell.m_is_number = false;
				}
			}
		}

		void ParseSfFootnoteMark()
		{
			m_footnotes.push_back(std::string());
			m_current_text_pointer = &m_footnotes.back();
		}

		void ParseSfFootnote()
		{
			std::ostringstream os;
			os << "(" << (1 + m_footnote_start + m_footnote_count) << ")";
			(*m_current_text_pointer) += os.str();
			++m_footnote_count;
		}

		void ParseSfPageStart()
		{
			if (m_footnote_count > 0 && m_footnote_start + m_footnote_count <= m_footnotes.size())
			{
				std::list<std::string>::iterator it = m_footnotes.begin();
				std::advance(it, m_footnote_start);
				for (int i = 0; i < m_footnote_count; ++i, ++it)
				{
					std::ostringstream os;
					os << "\n" << (1 + m_footnote_start + i) << ".";
					(*m_current_text_pointer) += os.str();
					(*m_current_text_pointer) += *it;
				}
				(*m_current_text_pointer) += "\n\n";
				m_footnote_start += m_footnote_count;
				m_footnote_count = 0;
			}
		}

		//std::map is much faster than a lot of "if" conditions
		void RegisterHandlers()
		{
			m_handlers["ls:workspace"] = &IWorkContent::ParseLsWorkspace;
			m_handlers["sf:p"] = &IWorkContent::ParseSfP;
			m_handlers["sf:br"] = &IWorkContent::ParseSfBr;
			m_handlers["sf:lnbr"] = &IWorkContent::ParseSfBr;
			m_handlers["sf:crbr"] = &IWorkContent::ParseSfBr;
			m_handlers["sf:contbr"] = &IWorkContent::ParseSfBr;
			m_handlers["sf:sectbr"] = &IWorkContent::ParseSfBr;
			m_handlers["sf:tab"] = &IWorkContent::ParseSfTab;
			m_handlers["sf:tabular-info"] = &IWorkContent::ParseSfTabularInfo;
			m_handlers["sf:tabular-model"] = &IWorkContent::ParseSfTabularModel;
			m_handlers["sf:position"] = &IWorkContent::ParseSfPosition;
			m_handlers["sf:rows"] = &IWorkContent::ParseSfRows;
			m_handlers["sf:columns"] = &IWorkContent::ParseSfColumns;
			m_handlers["sf:grid-row"] = &IWorkContent::ParseSfGridRow;
			m_handlers["sf:pm"] = &IWorkContent::ParseSfPm;
			m_handlers["sf:t"] = &IWorkContent::ParseSfT;
			m_handlers["sf:ct"] = &IWorkContent::ParseSfCT;
			m_handlers["sf:n"] = &IWorkContent::ParseSfN;
			m_handlers["sf:sl"] = &IWorkContent::ParseSfN;
			m_handlers["sf:st"] = &IWorkContent::ParseSfN;
			m_handlers["sf:s"] = &IWorkContent::ParseSfS;
			m_handlers["sf:cb"] = &IWorkContent::ParseSfS;
			m_handlers["sf:g"] = &IWorkContent::ParseSfS;
			m_handlers["sf:o"] = &IWorkContent::ParseSfS;
			m_handlers["sf:rd"] = &IWorkContent::ParseSfRd;
			m_handlers["sf:d"] = &IWorkContent::ParseSfD;
			m_handlers["sf:f"] = &IWorkContent::ParseSfF;
			m_handlers["sf:proxied-cell-ref"] = &IWorkContent::ParseSfProxiedCellRef;
			m_handlers["sf:rdu"] = &IWorkContent::ParseSfRdu;
			m_handlers["sf:du"] = &IWorkContent::ParseSfDu;
			m_handlers["sf:rn"] = &IWorkContent::ParseSfRn;
			m_handlers["sf:sticky-note"] = &IWorkContent::ParseSfStickyNote;
			m_handlers["sf:shape"] = &IWorkContent::ParseSfStickyNote;
			m_handlers["sf:cell-comment-drawable-info"] = &IWorkContent::ParseSfStickyNote;
			m_handlers["sf:chart-info"] = &IWorkContent::ParseSfChartInfo;
			m_handlers["sf:formula-chart-model"] = &IWorkContent::ParseSfFormulaChartModel;
			m_handlers["sf:string"] = &IWorkContent::ParseSfString;
			m_handlers["sf:cached_series_count"] = &IWorkContent::ParseSfCachedSeriesCount;
			m_handlers["sf:chart-name"] = &IWorkContent::ParseSfChartName;
			m_handlers["sf:chart-row_names"] = &IWorkContent::ParseSfChartRowNames;
			m_handlers["sf:chart-column_names"] = &IWorkContent::ParseSfChartColumnNames;
			m_handlers["sf:cell-style"] = &IWorkContent::ParseSfCellStyle;
			m_handlers["sf:date-format-ref"] = &IWorkContent::ParseSfDateFormatRef;
			m_handlers["sf:duration-format-ref"] = &IWorkContent::ParseSfDurationFormatRef;
			m_handlers["sf:date-format"] = &IWorkContent::ParseSfDateFormat;
			m_handlers["sf:duration-format"] = &IWorkContent::ParseSfDurationFormat;
			m_handlers["sf:number-format-ref"] = &IWorkContent::ParseSfNumberFormatRef;
			m_handlers["sf:number-format"] = &IWorkContent::ParseSfNumberFormat;
			m_handlers["key:slide"] = &IWorkContent::ParseKeySlide;
			m_handlers["sf:group"] = &IWorkContent::ParseSfGroup;
			m_handlers["key:title-placeholder"] = &IWorkContent::ParseKeyTitlePlaceholder;
			m_handlers["key:notes"] = &IWorkContent::ParseKeyNotes;
			m_handlers["sf:text-storage"] = &IWorkContent::ParseSfTextStorage;
			m_handlers["sf:annotation"] = &IWorkContent::ParseSfAnnotation;
			m_handlers["sf:annotation-field"] = &IWorkContent::ParseSfAnnotationField;
			m_handlers["sf:attachment"] = &IWorkContent::ParseSfAttachment;
			m_handlers["sf:attachment-ref"] = &IWorkContent::ParseSfAttachmentRef;
			m_handlers["sf:header"] = &IWorkContent::ParseSfHeader;
			m_handlers["sf:footer"] = &IWorkContent::ParseSfFooter;
			m_handlers["sf:footnotes"] = &IWorkContent::ParseSfFootnotes;
			m_handlers["sf:footnote-mark"] = &IWorkContent::ParseSfFootnoteMark;
			m_handlers["sf:footnote"] = &IWorkContent::ParseSfFootnote;
			m_handlers["sf:page-start"] = &IWorkContent::ParseSfPageStart;
		}

		void ParseXmlData()
		{
			while (true)
			{
				m_xml_reader->GetNextElement(m_current_element);
				if (m_current_element.m_kind == XmlReader::no_element)
					return;
				if (m_current_element.m_kind == XmlReader::characters)
				{
					if (m_reading_text)
					{
						if (m_iwork_type == numbers || m_iwork_type == pages)
							(*m_current_text_pointer) += m_current_element.m_characters;
						else if (m_iwork_type == key && (m_in_slide || m_in_title))
							(*m_current_text_pointer) += m_current_element.m_characters;
					}
					continue;
				}

				//associate xml element with handler, its quite fast
				std::map<std::string, XmlElementHandler>::iterator handler = m_handlers.find(m_current_element.m_name);
				if (handler != m_handlers.end())
				{
					m_xml_reader->ReadXmlAttributes(m_current_element);
					(this->*(handler->second))();
				}
				else
					m_xml_reader->SkipAttributes();
			};
		}
	};

	struct IWorkMetadataContent
	{
		typedef void (IWorkMetadataContent::*XmlElementHandler)(void);

		XmlReader* m_xml_reader;
		attributes::Metadata* m_metadata;
		bool m_in_metadata;
		bool m_in_authors;
		bool m_in_publication_info;
		bool m_in_creation_date;
		bool m_in_last_modify_date;
		int m_pages_count;
		XmlReader::XmlElement m_current_element;
		std::map<std::string, XmlElementHandler> m_handlers;
		std::string m_authors;
		std::string m_creation_date;
		std::string m_last_modify_date;

		IWorkMetadataContent(XmlReader& xml_reader, attributes::Metadata& metadata)
		{
			m_xml_reader = &xml_reader;
			m_metadata = &metadata;
			m_in_metadata = false;
			m_in_authors = false;
			m_in_last_modify_date = false;
			m_in_publication_info = false;
			m_in_creation_date = false;
			m_pages_count = 0;
			RegisterHandlers();
		}

		void ParseMetaData(const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
		{
			docwire_log(debug) << "Extracting metadata.";
			while (true)
			{
				m_xml_reader->GetNextElement(m_current_element);
				if (m_current_element.m_kind == XmlReader::no_element)
				{
					if (m_pages_count > 0)
					{
						m_metadata->page_count = m_pages_count;
					}
					if (m_authors.length() > 0)
					{
						std::vector<char> dest_authors(m_authors.length() * 2);
						size_t decoded_size = decode_html_entities_utf8(&dest_authors[0], m_authors.data());
						m_authors = std::string(&dest_authors[0], decoded_size);
						m_metadata->author = m_authors;
					}
					if (m_creation_date.length() > 0)
					{
						tm creation_date;
						if (string_to_date(m_creation_date, creation_date))
							m_metadata->creation_date = creation_date;
						else
							non_fatal_error_handler(make_error_ptr("Error occured during parsing date", m_creation_date));
					}
					if (m_last_modify_date.length() > 0)
					{
						tm last_modification_date;
						if (string_to_date(m_last_modify_date, last_modification_date))
							m_metadata->last_modification_date = last_modification_date;
						else
							non_fatal_error_handler(make_error_ptr("Error occured during parsing date", m_last_modify_date));
					}
					return;
				}
				if (m_current_element.m_kind == XmlReader::characters)
					continue;

				//associate xml element with handler, its quite fast
				std::map<std::string, XmlElementHandler>::iterator handler = m_handlers.find(m_current_element.m_name);
				if (handler != m_handlers.end())
				{
					m_xml_reader->ReadXmlAttributes(m_current_element);
					(this->*(handler->second))();
				}
				else
				{
					m_xml_reader->SkipAttributes();
				}
			};
		}

		void RegisterHandlers()
		{
			m_handlers["sf:page-start"] = &IWorkMetadataContent::ParseSfPageStart;
			m_handlers["ls:workspace"] = &IWorkMetadataContent::ParseLsWorkspace;
			m_handlers["key:slide"] = &IWorkMetadataContent::ParseSlSlide;
			m_handlers["sf:metadata"] = &IWorkMetadataContent::ParseSfMetadata;
			m_handlers["sf:authors"] = &IWorkMetadataContent::ParseSfAuthors;
			m_handlers["sf:string"] = &IWorkMetadataContent::ParseSfString;
			m_handlers["sl:publication-info"] = &IWorkMetadataContent::ParseSlPublicationInfo;
			m_handlers["sl:SLCreationDateProperty"] = &IWorkMetadataContent::ParseSlCreationDateProperty;
			m_handlers["sl:SLLastModifiedDateProperty"] = &IWorkMetadataContent::ParseSlLastModifiedDateProperty;
			m_handlers["sl:date"] = &IWorkMetadataContent::ParseSlDate;
		}

		void ParseSlDate()
		{
			std::string val;
			if (m_current_element.HasAttribute("fs:val"))
				val = m_current_element.m_attributes["sf:val"];
			if (val.length() > 0)
			{
				if (m_in_creation_date)
				{
					m_creation_date = val;
				}
				else if (m_in_last_modify_date)
				{
					m_last_modify_date = val;
				}
			}
		}

		void ParseSlLastModifiedDateProperty()
		{
			if (m_in_publication_info)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_in_last_modify_date = true;
				else
					m_in_last_modify_date = false;
			}
		}

		void ParseSlCreationDateProperty()
		{
			if (m_in_publication_info)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_in_creation_date = true;
				else
					m_in_creation_date = false;
			}
		}

		void ParseSlPublicationInfo()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
				m_in_publication_info = true;
			else
				m_in_publication_info = false;
		}

		void ParseSfString()
		{
			std::string str;
			if (m_current_element.HasAttribute("sfa:string"))
				str = m_current_element.m_attributes["sfa:string"];
			if (str.length() > 0)
			{
				if (m_in_authors)
				{
					if (m_authors.length() > 0)
						m_authors += "\n";
					m_authors += str;
				}
			}
		}

		void ParseSfAuthors()
		{
			if (m_in_metadata)
			{
				if (m_current_element.m_kind == XmlReader::opening_element)
					m_in_authors = true;
				else
					m_in_authors = false;
			}
		}

		void ParseLsWorkspace()
		{
			if (m_current_element.m_kind != XmlReader::closing_element)
				++m_pages_count;
		}

		void ParseSfPageStart()
		{
			if (m_current_element.m_kind != XmlReader::closing_element)
				++m_pages_count;
		}

		void ParseSlSlide()
		{
			if (m_current_element.m_kind != XmlReader::closing_element)
				++m_pages_count;
		}

		void ParseSfMetadata()
		{
			if (m_current_element.m_kind == XmlReader::opening_element)
				m_in_metadata = true;
			else
				m_in_metadata = false;
		}
	};

	void ReadMetadata(ZipReader& zipfile, attributes::Metadata& metadata, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
	{
		DataSource xml_data_source(zipfile, m_context_stack.top().m_xml_file);
		XmlReader xml_reader(xml_data_source);
		IWorkMetadataContent metadata_content(xml_reader, metadata);

		throw_if (!zipfile.loadDirectory());
		throw_if (getIWorkType(xml_reader) == IWorkContent::encrypted, errors::file_encrypted{});
		try
		{
			metadata_content.ParseMetaData(non_fatal_error_handler);
		}
		catch (const std::exception& ex)
		{
			std::throw_with_nested(make_error("ParseMetaData() failed"));
		}
	}

	IWorkContent::IWorkType getIWorkType(XmlReader& xml_reader)
	{
		XmlReader::XmlElement current_element;
		// warning TODO: Check how encrypted files really look like
		while (true)
		{
			try
			{
				xml_reader.GetNextElement(current_element);	//root element
			}
			catch (const std::exception& ex)
			{
				return IWorkContent::encrypted;	//encrypted or corrupted?
			}
			if (current_element.m_kind == XmlReader::opening_element)
			{
				if (current_element.m_name.substr(0, 3) == "sl:")
					return IWorkContent::pages;
				if (current_element.m_name.substr(0, 3) == "ls:")
					return IWorkContent::numbers;
				if (current_element.m_name.substr(0, 4) == "key:")
					return IWorkContent::key;
			}
			else if (current_element.m_kind == XmlReader::no_element)
				return IWorkContent::encrypted;
		}
	}

	void parseIWork(ZipReader& zipfile, std::string& text)
	{
		DataSource xml_data_source(zipfile, m_context_stack.top().m_xml_file);
		XmlReader xml_reader(xml_data_source);
		IWorkContent iwork_content(xml_reader);

		throw_if (!zipfile.loadDirectory());
		IWorkContent::IWorkType iwork_type = getIWorkType(xml_reader);
		throw_if (iwork_type == IWorkContent::encrypted, errors::file_encrypted{});
		iwork_content.setType(iwork_type);
		text.clear();
		try
		{
			iwork_content.ParseXmlData();
		}
		catch (const std::exception& ex)
		{
			std::throw_with_nested(make_error("ParseXmlData() failed"));
		}
		if (iwork_content.m_header.length() > 0)
		{
			if (iwork_content.m_header[iwork_content.m_header.length() - 1] != '\n')
				iwork_content.m_header += "\n";
		}
		if (iwork_content.m_text_body.length() > 0 && iwork_content.m_footer.length() > 0)
		{
			if (iwork_content.m_text_body[iwork_content.m_text_body.length() - 1] != '\n' && iwork_content.m_footer[0] != '\n')
				iwork_content.m_text_body += "\n";
		}
		iwork_content.m_text_body = iwork_content.m_header + iwork_content.m_text_body + iwork_content.m_footer;
		std::vector<char> dest_text(iwork_content.m_text_body.length() * 2);
		size_t decoded_size = decode_html_entities_utf8(&dest_text[0], iwork_content.m_text_body.data());
		text = std::string(&dest_text[0], decoded_size);
	}
};

IWorkParser::IWorkParser() = default;

void pimpl_impl<IWorkParser>::parse(const data_source& data, const emission_callbacks& emit_tag)
{
	docwire_log(debug) << "Using iWork parser.";
	scoped::stack_push<context> context_guard{m_context_stack, {.emit_tag = emit_tag}};
	ZipReader unzip{data};
	try
	{
		unzip.open();
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(make_error("ZipReader::unzip() failed"));
	}
	try
	{
		m_context_stack.top().m_xml_file = find_main_xml_file(unzip);
		emit_tag(tag::Document
			{
				.metadata=[this, &unzip, emit_tag]()
				{
					attributes::Metadata metadata;
					ReadMetadata(unzip, metadata, [emit_tag](std::exception_ptr e) { emit_tag(e); });
					return metadata;
				}
			});
		std::string text;
		parseIWork(unzip, text);
		emit_tag(tag::Text{.text=text});
		emit_tag(tag::CloseDocument{});
	}
	catch (const std::exception& ex)
	{
		std::throw_with_nested(make_error("Error parsing iWork main xml file"));
	}
}

continuation IWorkParser::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
	if (!std::holds_alternative<data_source>(tag))
		return emit_tag(std::move(tag));

	auto& data = std::get<data_source>(tag);
	data.assert_not_encrypted();

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
	{
		return emit_tag(std::move(tag));
	}

	impl().parse(data, emit_tag);

	return continuation::proceed;
}


} // namespace docwire
