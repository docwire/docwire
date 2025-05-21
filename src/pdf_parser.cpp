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
#include <leptonica/allheaders.h>
#include <mutex>
#include <pdfium/cpp/fpdf_scopers.h>
#include <pdfium/fpdf_doc.h>
#include <pdfium/fpdfview.h>
#include <pdfium/fpdf_text.h>
#include <pdfium/fpdf_edit.h>
#include <set>
#include <stack>
#include <stdlib.h>
#include <string.h>
#include "scoped_stack_push.h"
#include "throw_if.h"
#include <vector>
#include <zlib.h>
#include "charset_converter.h"

namespace docwire
{

namespace
{
	std::mutex pdfium_mutex;

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

using pix_unique_ptr = std::unique_ptr<PIX, decltype([](PIX* pix) { pixDestroy(&pix); })>;
using leptonica_data_ptr = std::unique_ptr<l_uint8, decltype(&lept_free)>;

pix_unique_ptr create_pix_from_fpdf_bitmap(int width, int height, int stride, int format, const unsigned char* pixels, l_int32 horizontal_dpi, l_int32 vertical_dpi)
{
	docwire_log_func_with_args(width, height, stride, format);
	pix_unique_ptr pix;

	if (format == FPDFBitmap_Gray)
	{
		pix = pix_unique_ptr{pixCreate(width, height, 8)};
		throw_if(!pix, "pixCreate failed for grayscale");
		pixSetXRes(pix.get(), horizontal_dpi);
		pixSetYRes(pix.get(), vertical_dpi);

		l_uint32 wpl = pixGetWpl(pix.get());
		l_uint32* pix_data = pixGetData(pix.get());
		for (int y = 0; y < height; ++y)
		{
			l_uint32* line = pix_data + y * wpl;
			const unsigned char* src_line = pixels + y * stride;
			for (int x = 0; x < width; ++x)
				SET_DATA_BYTE(line, x, src_line[x]);
		}
	}
	else if (format == FPDFBitmap_BGR || format == FPDFBitmap_BGRx)
	{
		pix = pix_unique_ptr{pixCreate(width, height, 32)};
		throw_if (!pix, "pixCreate failed for BGR/BGRx");
		pixSetXRes(pix.get(), horizontal_dpi);
		pixSetYRes(pix.get(), vertical_dpi);

		int bytes_per_pixel = (format == FPDFBitmap_BGRx) ? 4 : 3;
		l_uint32 wpl = pixGetWpl(pix.get());
		l_uint32* pix_data = pixGetData(pix.get());
		for (int y = 0; y < height; ++y)
		{
			const unsigned char* src_line = pixels + y * stride;
			l_uint32* line = pix_data + y * wpl;
			for (int x = 0; x < width; ++x)
			{
				l_uint8 b = src_line[x * bytes_per_pixel + 0];
				l_uint8 g = src_line[x * bytes_per_pixel + 1];
				l_uint8 r = src_line[x * bytes_per_pixel + 2];
				l_uint32 rgba_pixel = 0;
				throw_if(
					composeRGBAPixel(r, g, b, 255, &rgba_pixel) != 0,
					"composeRGBAPixel failed for BGR/BGRx pixel",
					x, y
				);
				line[x] = rgba_pixel;
			}
		}
	}
	else if (format == FPDFBitmap_BGRA)
	{
		pix = pix_unique_ptr{pixCreate(width, height, 32)};
		throw_if (!pix, "pixCreate failed for BGRA");
		pixSetXRes(pix.get(), horizontal_dpi);
		pixSetYRes(pix.get(), vertical_dpi);
		pixSetSpp(pix.get(), 4);

		l_uint32 wpl = pixGetWpl(pix.get());
		l_uint32* pix_data = pixGetData(pix.get());

		for (int y = 0; y < height; ++y)
		{
			const unsigned char* src_line = pixels + y * stride;
			l_uint32* line = pix_data + y * wpl;
			for (int x = 0; x < width; ++x)
			{
				l_uint8 b = src_line[x * 4 + 0];
				l_uint8 g = src_line[x * 4 + 1];
				l_uint8 r = src_line[x * 4 + 2];
				l_uint8 a = src_line[x * 4 + 3];
				l_uint32 rgba_pixel = 0;
				throw_if(
					composeRGBAPixel(r, g, b, a, &rgba_pixel) != 0,
					"composeRGBAPixel failed for BGRA pixel",
					x, y
				);
				line[x] = rgba_pixel;
			}
		}
	}
	else
	{
		throw make_error("Unsupported FPDFBitmap format for Pix creation", format, errors::uninterpretable_data{});
	}
	return pix;
}

using scoped_fpdf_document_with_custom_deleter = std::unique_ptr<
		std::remove_pointer_t<FPDF_DOCUMENT>,
		std::function<void(FPDF_DOCUMENT)>>;

struct context
{
	const emission_callbacks& emit_tag;
	scoped_fpdf_document_with_custom_deleter pdf_document;
};

} // unnamed namespace

template<>
struct pimpl_impl<PDFParser> : pimpl_impl_base
{
	std::stack<context> m_context_stack;

	continuation emit_tag(Tag&& tag)
	{
		return m_context_stack.top().emit_tag(std::move(tag));
	}

	continuation emit_tag_back(Tag&& tag)
	{
		return m_context_stack.top().emit_tag.back(std::move(tag));
	}

	FPDF_DOCUMENT pdf_document()
	{
		return m_context_stack.top().pdf_document.get();
	}

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
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		int page_count = FPDF_GetPageCount(pdf_document());
		docwire_log_var(page_count);
		for (size_t page_num = 0; page_num < page_count; page_num++)
		{
			docwire_log_var(page_num);
			auto response = emit_tag(tag::Page{});
			if (response == continuation::skip)
			{
				continue;
			}
			else if (response == continuation::stop)
			{
				break;
			}
			try
			{
				PageText page_text;
				ScopedFPDFPage page { FPDF_LoadPage(pdf_document(), page_num) };
				throw_if(!page);
				ScopedFPDFTextPage text_page { FPDFText_LoadPage(page.get()) };
				throw_if(!text_page);
				int object_count = FPDFPage_CountObjects(page.get());
				throw_if (object_count < 0);
				charset_converter conv("UTF-16LE", "UTF-8"); // Create converter *outside* the loops
				bool stop_processing = false;
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
							ScopedFPDFBitmap bitmap { FPDFImageObj_GetBitmap(object) };
							int width = FPDFBitmap_GetWidth(bitmap.get());
							int height = FPDFBitmap_GetHeight(bitmap.get());
							int stride = FPDFBitmap_GetStride(bitmap.get());
							const unsigned char* pixels = (const unsigned char*)FPDFBitmap_GetBuffer(bitmap.get());
        					int format = FPDFBitmap_GetFormat(bitmap.get());

							FPDF_IMAGEOBJ_METADATA image_metadata;
							l_int32 h_res = 72; // Default DPI
							l_int32 v_res = 72;   // Default DPI
							if (FPDFImageObj_GetImageMetadata(object, page.get(), &image_metadata)) {
								if (image_metadata.horizontal_dpi > 0.0f)
									h_res = static_cast<l_int32>(image_metadata.horizontal_dpi);
								if (image_metadata.vertical_dpi > 0.0f)
									v_res = static_cast<l_int32>(image_metadata.vertical_dpi);
							}

            				pix_unique_ptr pix = create_pix_from_fpdf_bitmap(width, height, stride, format, pixels, h_res, v_res);
			                l_uint8* png_data_raw = nullptr;
            			    size_t png_size = 0;
                			leptonica_data_ptr png_data(nullptr, lept_free);
                			throw_if (pixWriteMemPng(&png_data_raw, &png_size, pix.get(), 0.0f) != 0);
							throw_if (!png_data_raw);
							throw_if (png_size <= 0);
                    		png_data.reset(png_data_raw);
							std::vector<std::byte> image_data;
                    		image_data.resize(png_size);
							memcpy(image_data.data(), png_data.get(), png_size);
                			data_source image_source(std::move(image_data), mime_type { "image/png" }, confidence::highest);
							tag::Image image{.source = image_source};
							if (emit_tag_back(std::move(image)) == continuation::stop)
								stop_processing = true;
							break;
						}
						default:
							break;
					}
				}
				if (stop_processing)
					break;
				std::string single_page_text;
				page_text.getText(single_page_text);
				single_page_text += "\n\n";
				auto response = emit_tag(tag::Text{single_page_text});
				if (response == continuation::stop)
				{
					break;
				}
        		auto response2 = emit_tag(tag::ClosePage{});
        		if (response2 == continuation::stop)
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
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		unsigned long buffer_size = FPDF_GetMetaText(pdf_document(), tag.c_str(), nullptr, 0);
		throw_if(buffer_size < 2);
		std::vector<unsigned short> buffer(buffer_size);
		charset_converter conv("UTF-16LE", "UTF-8");
		unsigned long bytes_returned = FPDF_GetMetaText(pdf_document(), tag.c_str(), buffer.data(), buffer.size());
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
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		metadata.page_count = FPDF_GetPageCount(pdf_document());
	}

	void init_pdfium_once()
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
				std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
				FPDF_InitLibraryWithConfig(&config);
			}

			~pdfium_lifecycle_manager()
			{
				std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
				FPDF_DestroyLibrary();
			}
		};
		static pdfium_lifecycle_manager pdfium_manager;
	}

	void loadDocument(const data_source& data)
	{
		docwire_log_func();
		std::span<const std::byte> span = data.span();
		init_pdfium_once();
		std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
		m_context_stack.top().pdf_document = scoped_fpdf_document_with_custom_deleter
			{
				FPDF_LoadMemDocument(span.data(), span.size(), nullptr),
				[&](FPDF_DOCUMENT doc)
				{
					std::lock_guard<std::mutex> pdfium_mutex_lock(pdfium_mutex);
					FPDF_CloseDocument(doc);
				}
			};
		if (!pdf_document())
		{
			if (FPDF_GetLastError() == FPDF_ERR_PASSWORD)
				throw make_error(errors::file_encrypted{});
			else
				throw make_error("FPDF_LoadMemDocument() failed", FPDF_GetLastError(), errors::uninterpretable_data{});
		}
	}

	attributes::Metadata metaData(const data_source& data);
	void parse(const data_source& data, const emission_callbacks& emit_tag);
};

PDFParser::PDFParser() = default;

attributes::Metadata pimpl_impl<PDFParser>::metaData(const data_source& data)
{
	attributes::Metadata metadata;
	parseMetadata(metadata);
	return metadata;
}

void pimpl_impl<PDFParser>::parse(const data_source& data, const emission_callbacks& emit_tag)
{
	docwire_log(debug) << "Using PDF parser.";
	scoped::stack_push<context> context_guard{m_context_stack, {.emit_tag = emit_tag}};
	loadDocument(data);
	emit_tag(tag::Document
		{
			.metadata = [this, &data]()

			{
				return metaData(data);
			}
		});
	parseText();
	emit_tag(tag::CloseDocument{});
}

continuation PDFParser::operator()(Tag&& tag, const emission_callbacks& emit_tag)
{
	if (!std::holds_alternative<data_source>(tag))
		return emit_tag(std::move(tag));

	auto& data = std::get<data_source>(tag);
	data.assert_not_encrypted();

	static const std::vector<mime_type> supported_mime_types = {
		mime_type{"application/pdf"}
	};

	if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
		return emit_tag(std::move(tag));

	try
	{
		impl().parse(data, emit_tag);
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("PDF parsing failed"));
	}
	return continuation::proceed;
}

} // namespace docwire
