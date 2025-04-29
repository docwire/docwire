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

#include "pdf_parser.h"

#include "error_tags.h"
#include "log.h"
#include "make_error.h"
#include <mutex>
#include <pdfium/cpp/fpdf_scopers.h>
#include <pdfium/fpdf_doc.h>
#include <pdfium/fpdfview.h>
#include <pdfium/fpdf_text.h>
#include <pdfium/fpdf_edit.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include "throw_if.h"
#include <vector>
#include <zlib.h>
#include "charset_converter.h"

namespace docwire
{

namespace
{
	std::mutex pdfium_mutex;
} // unnamed namespace

void parsePDFDate(tm& date, const std::string& str_date)
{
	if (str_date.length() < 14)
		return;
	std::string year = str_date.substr(0, 4);
	date.tm_year = strtol(year.c_str(), NULL, 10);
	std::string month = str_date.substr(4, 2);
	date.tm_mon = strtol(month.c_str(), NULL, 10);
	std::string day = str_date.substr(6, 2);
	date.tm_mday = strtol(day.c_str(), NULL, 10);
	std::string hour = str_date.substr(8, 2);
	date.tm_hour = strtol(hour.c_str(), NULL, 10);
	std::string minute = str_date.substr(10, 2);
	date.tm_min = strtol(minute.c_str(), NULL, 10);
	std::string second = str_date.substr(12, 2);
	date.tm_sec = strtol(second.c_str(), NULL, 10);
	date.tm_year -= 1900;
	--date.tm_mon;
}

template<>
struct pimpl_impl<PDFParser> : with_pimpl_owner<PDFParser>
{
	pimpl_impl(PDFParser& owner) : with_pimpl_owner{owner} {}
	ScopedFPDFDocument m_pdf_document;

		struct PageText
		{
			struct TextElement
			{
				std::string m_text;
				double m_x, m_y, m_width, m_height;
				double m_space_size;

				TextElement(double x, double y, double w, double h, double space_size, const std::string& text)
				{
					docwire_log_func_with_args(x, y, w, h, space_size, text);
					// warning TODO: We have position and size for each string. We can use those values to improve parser
					m_x = correctSize(x);
					m_y = correctSize(y);
					m_text = text;
					m_width = correctSize(w);
					m_height = correctSize(h);
					m_space_size = space_size;
				}

				double correctSize(double value)
				{
					//file may be corrupted, we should set some maximum values.
					// warning TODO: Check MediaBox entry (defines page area)
					if (value < 0)
						value = 0.0;
					if (value > 5000)
						value = 5000;
					return value;
				}

				bool operator == (const TextElement& compared) const
				{
					return compared.m_y == m_y && compared.m_x == m_x;
				}

				bool operator < (const TextElement& compared) const
				{
					if (abs(int(m_y - compared.m_y)) > 4.0)	//tolerace
					{
						return m_y > compared.m_y;
					}
					return m_x < compared.m_x;
				}

				bool operator > (const TextElement& compared) const
				{
					if (abs(int(m_y - compared.m_y)) > 4.0) //tolerace
					{
						return m_y < compared.m_y;
					}
					return m_x > compared.m_x;
				}

				void log_to_record_stream(log_record_stream& s) const
				{
					s << docwire_log_streamable_obj(*this, m_text, m_x, m_y, m_width, m_height, m_space_size);
				}
			};

			std::multiset<TextElement> m_text_elements;

			void getText(std::string& output)
			{
				// warning TODO: For now we are sorting strings using their x and y positions. Maybe we should implement better algorithms.
				std::multiset<TextElement>::iterator it = m_text_elements.begin();
				bool first = true;
				double x_end, y, x_begin;
				while (it != m_text_elements.end())
				{
					docwire_log_var(*it);
					//some minimum values for new line and space. Calculated experimentally
					double new_line_size = (*it).m_height * 0.75 < 4.0 ? 4.0 : (*it).m_height * 0.75;

          double horizontal_lines_separator_size = (*it).m_height;
					docwire_log_vars(new_line_size, horizontal_lines_separator_size, first);
					if (!first)
					{
						double dx = (*it).m_x - x_end;
						double dy = y - ((*it).m_y + (*it).m_height / 2);
						docwire_log_vars(dx, dy);

						if (dy >= new_line_size)
						{
							while (dy >= new_line_size)
							{
								docwire_log(debug) << "New line because of y position difference" << docwire_log_streamable_vars(dy, new_line_size);
								output += '\n';
								dy -= new_line_size;
							}
						}
						else if ((*it).m_x < x_begin)	//force new line
						{
							docwire_log(debug) << "New line because of x position difference" << docwire_log_streamable_vars(it->m_x, x_begin);
							output += '\n';
						}
						else if (dx >= (*it).m_space_size)
						{
						  if (dx > horizontal_lines_separator_size)
						  {
						    output += "\t\t\t\t";
						  }
						  else if (dx >= (*it).m_space_size)
						  {
						    output += ' ';
						  }
						}
					}
					output += (*it).m_text;
					first = false;
					x_begin = (*it).m_x;
					x_end = x_begin + (*it).m_width;
					y = (*it).m_y + (*it).m_height / 2;
					++it;
				}
			}

			PageText()
			{
			}
		};

	void parseText()
	{
		docwire_log_func();
		int page_count = FPDF_GetPageCount(m_pdf_document.get());
		docwire_log_var(page_count);
		for (size_t page_num = 0; page_num < page_count; page_num++)
		{
			docwire_log_var(page_num);
			auto response = owner().sendTag(tag::Page{});
			if (response.skip)
			{
				continue;
			}
			if (response.cancel)
			{
				break;
			}
			try
			{
				PageText page_text;
				ScopedFPDFPage page { FPDF_LoadPage(m_pdf_document.get(), page_num) };
				throw_if(!page);
				ScopedFPDFTextPage text_page { FPDFText_LoadPage(page.get()) };
				throw_if(!text_page);
				int object_count = FPDFPage_CountObjects(page.get());
				throw_if (object_count < 0);
				charset_converter conv("UTF-16LE", "UTF-8"); // Create converter *outside* the loops
				for (int i = 0; i < object_count; ++i)
				{
    				FPDF_PAGEOBJECT object = FPDFPage_GetObject(page.get(), i);
    				throw_if (!object);
    				int object_type = FPDFPageObj_GetType(object);
    				switch (object_type)
					{
        				case FPDF_PAGEOBJ_TEXT:
        				{
							unsigned long buffer_size = FPDFTextObj_GetText(object, text_page.get(), nullptr, 0);
            				throw_if(buffer_size < 2);
                			std::vector<unsigned short> buffer(buffer_size);
                			unsigned long bytes_returned = FPDFTextObj_GetText(object, text_page.get(), buffer.data(), buffer.size());
                			throw_if(bytes_returned != buffer_size);
                    		std::string utf8_text = conv.convert(std::string{
                        		reinterpret_cast<const char*>(buffer.data()),
                        		buffer_size - sizeof(unsigned short) // Exclude NULL terminator
                    		});

							float left, bottom, right, top;
							throw_if(!FPDFPageObj_GetBounds(object, &left, &bottom, &right, &top));

							float space_size = 0.0f; // Default space size if calculation fails
							FPDF_FONT font = FPDFTextObj_GetFont(object);
							if (font)
							{
								float font_size;
								if (FPDFTextObj_GetFontSize(object, &font_size) && font_size > 0)
            						space_size = font_size / 3.0f;
								else
            						docwire_log(warning) << "Failed to get font size for text object.";
    						}
							else
        						docwire_log(warning) << "Failed to get font for text object.";
							PageText::TextElement new_element(left, bottom, right - left, top - bottom, space_size, utf8_text);
							page_text.m_text_elements.insert(new_element);
            				break;
        				}
						case FPDF_PAGEOBJ_IMAGE:
						{
							break;
						}
						default:
							break;
					}
				}
				std::string single_page_text;
				page_text.getText(single_page_text);
				single_page_text += "\n\n";
				auto response = owner().sendTag(tag::Text{single_page_text});
				if (response.cancel)
				{
					break;
				}
        		auto response2 = owner().sendTag(tag::ClosePage{});
        		if (response2.cancel)
        		{
          			break;
        		}
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(make_error(page_num));
			}
			docwire_log(debug) << "Page processed" << docwire_log_streamable_var(page_num);
		}
	}

	std::string get_meta_text(const std::string& tag)
	{
		unsigned long buffer_size = FPDF_GetMetaText(m_pdf_document.get(), tag.c_str(), nullptr, 0);
		throw_if(buffer_size < 2);
		std::vector<unsigned short> buffer(buffer_size);
		charset_converter conv("UTF-16LE", "UTF-8");
		unsigned long bytes_returned = FPDF_GetMetaText(m_pdf_document.get(), tag.c_str(), buffer.data(), buffer.size());
		throw_if(bytes_returned != buffer_size);
		std::string utf8_text = conv.convert(std::string{
			reinterpret_cast<const char*>(buffer.data()),
			buffer_size - sizeof(unsigned short) // Exclude NULL terminator
		});
		return utf8_text;
	}

	void parseMetadata(attributes::Metadata& metadata)
	{
		std::string author_str = get_meta_text("Author");
		if (!author_str.empty())
			metadata.author = author_str;
		std::string creation_date_str = get_meta_text("CreationDate");
		if (!creation_date_str.empty())
		{
			tm creation_date_tm;
			int offset = 0;
			while (creation_date_str.length() > offset && (creation_date_str[offset] < '0' || creation_date_str[offset] > '9'))
				++offset;
			creation_date_str.erase(0, offset);
			parsePDFDate(creation_date_tm, creation_date_str);
			metadata.creation_date = creation_date_tm;
		}
		std::string mod_date_str = get_meta_text("ModDate");
		if (!mod_date_str.empty())
		{
			tm modify_date_tm;
			int offset = 0;
			while (mod_date_str.length() > offset && (mod_date_str[offset] < '0' || mod_date_str[offset] > '9'))
				++offset;
			mod_date_str.erase(0, offset);
			parsePDFDate(modify_date_tm, mod_date_str);
			metadata.last_modification_date = modify_date_tm;
		}
		metadata.page_count = FPDF_GetPageCount(m_pdf_document.get());
	}

	void init_pdfium()
	{
		class pdfium_lifecycle_manager
		{
		public:
			pdfium_lifecycle_manager()
			{
				FPDF_LIBRARY_CONFIG config;
				config.version = 2;
				config.m_pUserFontPaths = nullptr;
				config.m_pIsolate = nullptr;
				config.m_v8EmbedderSlot = 0;
				FPDF_InitLibraryWithConfig(&config);
			}

			~pdfium_lifecycle_manager()
			{
				FPDF_DestroyLibrary();
			}
		};
		static pdfium_lifecycle_manager pdfium_manager;
	}

	void loadDocument(const data_source& data)
	{
		docwire_log_func();
		init_pdfium();
		std::span<const std::byte> span = data.span();
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		m_pdf_document = ScopedFPDFDocument { FPDF_LoadMemDocument(span.data(), span.size(), nullptr) };
		if (!m_pdf_document)
		{
			if (FPDF_GetLastError() == FPDF_ERR_PASSWORD)
				throw make_error(errors::file_encrypted{});
			else
				throw make_error("FPDF_LoadMemDocument() failed", FPDF_GetLastError(), errors::uninterpretable_data{});
		}
	}
};

PDFParser::PDFParser()
	: with_pimpl<PDFParser>(nullptr)
{
	std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
	renew_impl();
}

PDFParser::~PDFParser()
{
	std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
	destroy_impl();
}

attributes::Metadata PDFParser::metaData(const data_source& data)
{
	attributes::Metadata metadata;
	impl().loadDocument(data);
	impl().parseMetadata(metadata);
	return metadata;
}

void
PDFParser::parse(const data_source& data)
{
	docwire_log(debug) << "Using PDF parser.";
	{
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		renew_impl();
	}
	sendTag(tag::Document
		{
			.metadata = [this, &data]()
			{
				return metaData(data);
			}
		});
	impl().loadDocument(data);
	{
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		impl().parseText();
	}
	sendTag(tag::CloseDocument{});
}

} // namespace docwire
