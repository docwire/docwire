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

#include <cmath>
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
#include "tags.h"
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

const std::vector<mime_type> supported_mime_types =
{
	mime_type{"application/pdf"}
};

using PageElementVariant = std::variant<tag::Text, tag::Image>;

// Helper to get a characteristic height for an element, prioritizing font_size for text.
double get_element_characteristic_height(const PageElementVariant& element_variant) {
    return std::visit([](const auto& el) -> double {
        double h = 10.0; // Default height if no other info
        if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Text>) {
            if (el.font_size && *el.font_size > 0) {
                h = *el.font_size; // Prioritize font_size for text
            } else if (el.position.height && *el.position.height > 0) {
                h = *el.position.height;
            }
        } else if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Image>) {
            if (el.position.height && *el.position.height > 0) {
                h = *el.position.height;
            }
        }
        return std::max(1.0, h); // Ensure at least 1.0
    }, element_variant);
}

struct PageElementVariantComparator
{
	bool operator()(const PageElementVariant& lhs, const PageElementVariant& rhs) const
	{
		auto get_y = [](const auto& el) -> std::optional<double> {
			return el.position.y;
		};
		auto get_x = [](const auto& el) -> std::optional<double> {
			return el.position.x;
		};

		std::optional<double> y_lhs_opt = std::visit(get_y, lhs);
		std::optional<double> y_rhs_opt = std::visit(get_y, rhs);

		if (!y_lhs_opt && !y_rhs_opt) return false; 
		if (!y_lhs_opt) return false; 
		if (!y_rhs_opt) return true;  

		// Calculate an adaptive y_tolerance
		double h_lhs = get_element_characteristic_height(lhs);
		double h_rhs = get_element_characteristic_height(rhs);
		// Use a fraction of the max characteristic height. 40% seems like a reasonable starting point.
		// This means their bottom y-coordinates can differ by up to this amount.
		double adaptive_y_tolerance = std::max(h_lhs, h_rhs) * 0.40;
		adaptive_y_tolerance = std::max(2.0, adaptive_y_tolerance); // Ensure a minimum absolute tolerance

		if (std::abs(*y_lhs_opt - *y_rhs_opt) > adaptive_y_tolerance) {
			return *y_lhs_opt > *y_rhs_opt; // Higher Y (more top on page) comes first
		}

		std::optional<double> x_lhs_opt = std::visit(get_x, lhs);
		std::optional<double> x_rhs_opt = std::visit(get_x, rhs);

		if (!x_lhs_opt && !x_rhs_opt) return false;
		if (!x_lhs_opt) return false;
		if (!x_rhs_opt) return true;

		return *x_lhs_opt < *x_rhs_opt; // Then left-to-right
	}
};

bool ends_with_whitespace(const std::string& s) {
	return !s.empty() && std::isspace(static_cast<unsigned char>(s.back()));
}
	
bool begins_with_whitespace(const std::string& s) {
	return !s.empty() && std::isspace(static_cast<unsigned char>(s.front()));
}

} // anonymous namespace
	
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
				std::multiset<PageElementVariant, PageElementVariantComparator> page_elements;
				ScopedFPDFPage page { FPDF_LoadPage(pdf_document(), page_num) };
				throw_if(!page);
				// text_page is only needed for FPDFTextObj_GetText, so load it if/when a text object is found.
				ScopedFPDFTextPage text_page { nullptr };

				int object_count = FPDFPage_CountObjects(page.get());
				throw_if (object_count < 0, "FPDFPage_CountObjects returned negative count");
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
							if (!text_page) { // Load text_page on demand
								text_page.reset(FPDFText_LoadPage(page.get()));
								throw_if(!text_page, "FPDFText_LoadPage failed");
							}
							unsigned long buffer_size = FPDFTextObj_GetText(object, text_page.get(), nullptr, 0);
            				std::string utf8_text;
							if (buffer_size > 0) { // FPDFTextObj_GetText needs at least 2 bytes for empty string (null terminator)
                				std::vector<unsigned short> buffer(buffer_size / sizeof(unsigned short)); // buffer_size is in bytes
                				unsigned long bytes_returned = FPDFTextObj_GetText(object, text_page.get(), buffer.data(), buffer_size);
                				throw_if(bytes_returned > buffer_size || (bytes_returned == 0 && buffer_size >0) , "FPDFTextObj_GetText failed to retrieve text or returned unexpected size");
                    			if (bytes_returned > 0) { // bytes_returned includes the null terminator(s)
									utf8_text = conv.convert(std::string{
										reinterpret_cast<const char*>(buffer.data()),
										bytes_returned - sizeof(unsigned short) // Exclude UTF-16LE NULL terminator
									});
								}
							}

							float left, bottom, right, top;
							throw_if(!FPDFPageObj_GetBounds(object, &left, &bottom, &right, &top));

							float font_size_val = 0.0f;
							FPDF_FONT font = FPDFTextObj_GetFont(object);
							if (font)
							{
								if (!FPDFTextObj_GetFontSize(object, &font_size_val) || font_size_val <= 0) {
            						docwire_log(warning) << "Failed to get font size for text object.";
									font_size_val = 10.0f; // Default if not found
								}
    						}
							else
        						docwire_log(warning) << "Failed to get font for text object.";
							page_elements.insert(tag::Text{
								.text = utf8_text,
								.position = {
									.x = std::optional<double>{static_cast<double>(left)},
									.y = std::optional<double>{static_cast<double>(bottom)},
									.width = std::optional<double>{static_cast<double>(right - left)},
									.height = std::optional<double>{static_cast<double>(top - bottom)}
								},
								.font_size = static_cast<double>(font_size_val)
							});
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

							float left, bottom, right, top;
							throw_if(!FPDFPageObj_GetBounds(object, &left, &bottom, &right, &top));
							page_elements.insert(tag::Image{
                                .source = std::move(image_source),
                                .alt = std::nullopt, // PDFium does not easily provide this for FPDF_PAGEOBJ_IMAGE
                                .position = {
                                    .x = std::optional<double>{static_cast<double>(left)},
                                    .y = std::optional<double>{static_cast<double>(bottom)},
                                    .width = std::optional<double>{static_cast<double>(right - left)},
                                    .height = std::optional<double>{static_cast<double>(top - bottom)}
                                }
                            });
							break;
						}
						default:
							break;
					}
					if (stop_processing) break;
				}
				if (stop_processing)
					break;

				const PageElementVariant* prev_element_variant = nullptr;
				for (const auto& element : page_elements)
				{
					if (prev_element_variant)
					{
						std::visit(
							[&](const auto& prev_el_concrete) {
								std::visit(
									[&](const auto& current_el_concrete) {
										// Ensure all elements have necessary positional attributes
										if (!prev_el_concrete.position.y || !prev_el_concrete.position.height ||
											!prev_el_concrete.position.x || !prev_el_concrete.position.width ||
											!current_el_concrete.position.y || !current_el_concrete.position.height ||
											!current_el_concrete.position.x) {
											// If either element lacks position info, skip detailed spacing logic.
											return;
										}

										double prev_y_center = *prev_el_concrete.position.y + *prev_el_concrete.position.height / 2.0;
										double current_y_center = *current_el_concrete.position.y + *current_el_concrete.position.height / 2.0;
										double y_diff = prev_y_center - current_y_center;

										// Helper to determine a reasonable space threshold based on element properties
										auto get_space_threshold = [](const auto& el) -> double {
											double threshold_val = 2.0; // Default small threshold if other properties are missing
											if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Text>) {
												if (el.font_size && *el.font_size > 0) {
													threshold_val = *el.font_size / 3.5; // Approx 1/3.5 of font size
												} else if (el.position.height && *el.position.height > 0) {
													threshold_val = *el.position.height / 3.0; // Approx 1/3 of height as fallback
												}
											} else if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Image>) {
												if (el.position.height && *el.position.height > 0) {
													threshold_val = *el.position.height / 4.0; // Heuristic for images
												}
											}
											return std::max(1.0, threshold_val); // Ensure threshold is at least 1.0pt
										};

										auto get_effective_line_height = [](const auto& el) -> double {
											double h = 10.0; // Default height
											if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Text>) {
												if (el.font_size && *el.font_size > 0) h = *el.font_size;
												else if (el.position.height && *el.position.height > 0) h = *el.position.height;
											} else if constexpr (std::is_same_v<std::decay_t<decltype(el)>, tag::Image>) {
												if (el.position.height && *el.position.height > 0) h = *el.position.height;
											}
											return std::max(1.0, h); // Ensure at least 1.0
										};

										double prev_eff_h = get_effective_line_height(prev_el_concrete);
										double curr_eff_h = get_effective_line_height(current_el_concrete);
										double max_relevant_line_height = std::max(prev_eff_h, curr_eff_h);
										
										// Threshold for needing at least one newline
										double single_newline_threshold = max_relevant_line_height * 0.65;

										if (y_diff > single_newline_threshold) {
											int num_newlines_to_emit = static_cast<int>(std::round(y_diff / max_relevant_line_height));
											if (num_newlines_to_emit < 1) num_newlines_to_emit = 1;
											for (int k = 0; k < num_newlines_to_emit; ++k) {
												if (emit_tag(tag::BreakLine{}) == continuation::stop) { stop_processing = true; break; }
											}
										} else if (*current_el_concrete.position.x < *prev_el_concrete.position.x && std::abs(y_diff) < single_newline_threshold) {
											if (emit_tag(tag::BreakLine{}) == continuation::stop) { stop_processing = true; }
										} else if (std::holds_alternative<tag::Text>(*prev_element_variant) && std::holds_alternative<tag::Text>(element)) {
											const auto& prev_text_el = std::get<tag::Text>(*prev_element_variant);
											const auto& current_text_el = std::get<tag::Text>(element);
											// Ensure necessary fields have values
											if (!prev_text_el.position.x || !prev_text_el.position.width || !current_text_el.position.x) return;

											double space_threshold = get_space_threshold(current_text_el); // Base threshold on current element
											double x_gap = *current_text_el.position.x - (*prev_text_el.position.x + *prev_text_el.position.width);
											if (x_gap > space_threshold &&
												!ends_with_whitespace(prev_text_el.text) &&
												!begins_with_whitespace(current_text_el.text)) {
												if (emit_tag(tag::Text{" "}) == continuation::stop) { stop_processing = true; }
											}
										} else if (prev_element_variant->index() != element.index() && std::abs(y_diff) < single_newline_threshold) {
											// Different types (Text and Image) on the same visual line
											// Ensure necessary fields have values
											if (!prev_el_concrete.position.x || !prev_el_concrete.position.width ||
												!current_el_concrete.position.x) {
												return;
											}
											// Use the threshold of the preceding element to decide if a space is needed
											double space_threshold = get_space_threshold(prev_el_concrete);
											double x_gap = *current_el_concrete.position.x - (*prev_el_concrete.position.x + *prev_el_concrete.position.width);
											if (x_gap > space_threshold) {
												bool add_space_flag = true;
												// Check if previous element is Text and ends with space
												if constexpr (std::is_same_v<std::decay_t<decltype(prev_el_concrete)>, tag::Text>) {
													if (ends_with_whitespace(prev_el_concrete.text)) {
														add_space_flag = false;
													}
												}
												// Check if current element is Text and begins with space (only if not already forbidden)
												if (add_space_flag) {
													if constexpr (std::is_same_v<std::decay_t<decltype(current_el_concrete)>, tag::Text>) {
														if (begins_with_whitespace(current_el_concrete.text)) {
															add_space_flag = false;
														}
													}
												}
												if (add_space_flag) {
													if (emit_tag(tag::Text{" "}) == continuation::stop) { stop_processing = true; }
												}
											}
										}
									}, element
								);
							}, *prev_element_variant
						);
						if (stop_processing) break;
					}

					std::visit([&](auto&& concrete_element) {
						if (emit_tag(std::move(concrete_element)) == continuation::stop) { stop_processing = true; }
					}, PageElementVariant{element}); // Copy to move from const multiset element

					if (stop_processing) break;
					prev_element_variant = &element;
				}
				if (stop_processing) break;

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
