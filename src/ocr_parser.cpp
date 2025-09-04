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

#include "ocr_parser.h"

#include "document_elements.h"
#include "error_tags.h"
#include <leptonica/allheaders.h>
#include <leptonica/array_internal.h>
#include <leptonica/pix_internal.h>
#include <stack>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <filesystem>
#include <cstdlib>
#include <magic_enum/magic_enum_iostream.hpp>
#include "log.h"
#include "lru_memory_cache.h"
#include <mutex>
#include <numeric>
#include "resource_path.h"
#include "scoped_stack_push.h"
#include <tesseract/resultiterator.h>
#include "throw_if.h"

namespace docwire
{

namespace
{

class leptonica_stderr_capturer
{
public:
    leptonica_stderr_capturer()
    {
        leptSetStderrHandler(stderr_handler);
    }

    ~leptonica_stderr_capturer()
    {
        leptSetStderrHandler(nullptr);
    }

    std::string contents()
    {
        return boost::algorithm::trim_right_copy(m_contents);
    }

private:
    static void stderr_handler(const char* msg)
    {
        m_contents += msg;
    }

    static thread_local std::string m_contents;
};

thread_local std::string leptonica_stderr_capturer::m_contents;

struct context
{
    const message_callbacks& emit_message;
};

} // anonymous namespace

template<>
struct pimpl_impl<OCRParser> : with_pimpl_owner<OCRParser>
{
    pimpl_impl(OCRParser& owner) : with_pimpl_owner{owner} {}
    std::vector<Language> m_languages;
    ocr_confidence_threshold m_ocr_confidence_threshold;
    ocr_timeout m_ocr_timeout;
    ocr_data_path m_ocr_data_path;
    std::stack<context> m_context_stack;

    template <typename T>
	continuation emit_message(T&& object) const
	{
		return m_context_stack.top().emit_message(std::forward<T>(object));
	}

    static bool cancel (void* data, int words)
    {
        auto context_ptr = reinterpret_cast<context*>(data);
        return context_ptr->emit_message(ocr::PleaseWait{}) == continuation::stop;
    }
};  

Pix* pixToGrayscale(Pix* pix)
{
    Pix* output{};
    switch(pix->d)
    {
    case 8:
        output = pixRemoveColormap(pix, REMOVE_CMAP_TO_GRAYSCALE); 
        break;
    case 16:
        {
            auto tmp = pixConvert16To8(pix, 0);
            output = pixRemoveColormap(tmp, REMOVE_CMAP_TO_GRAYSCALE); 
            pixDestroy(&tmp);
            break;
        }
    case 32:
        {
            auto tmp = pixRemoveAlpha(pix);
            output = pixConvertRGBToGrayFast(tmp);
            pixDestroy(&tmp);
            break;	
        }
    default:
        throw make_error("Format not supported", pix->d, errors::uninterpretable_data{});
    }
    return output;
}

Pix* binarizePix(Pix* pix)
{
    Pix* temp{ NULL };
    // 200x200 - size of the binarized tiles; 0, 0 - no smoothing; 0.1 - typical scorefract
    pixOtsuAdaptiveThreshold(pix, 200, 200, 0, 0, 0.1, NULL, &temp);
    return temp;
}

Pix* scalePix(Pix* pix)
{
    Pix* scaled{ nullptr };
    if(pix->xres && pix->yres)
    {
        // Calculate the scale-factors
        constexpr float desired{ 301.f };
        float xScaleFactor{}, yScaleFactor{};

        xScaleFactor = desired / pix->xres;
        yScaleFactor = desired / pix->yres;

        scaled = pixScaleGrayLI(pix, xScaleFactor, yScaleFactor);
    }
    else
    {
        // For corrupted files
        scaled = pixScale(pix, 4, 4);
    }
    return scaled;
}

namespace
{
    using tesseract::TessBaseAPI;
    auto tessAPIDeleter = [](TessBaseAPI* tessAPI)
    {
        tessAPI->End();
        delete tessAPI;
    };
    using tessAPIWrapper = std::unique_ptr<TessBaseAPI, decltype(tessAPIDeleter)>;
}

using magic_enum::ostream_operators::operator<<;

namespace
{
    std::mutex tesseract_libtiff_mutex;

    using pix_unique_ptr = std::unique_ptr<PIX, decltype([](PIX* pix) { pixDestroy(&pix); })>;

std::shared_ptr<PIX> load_pix(const data_source& data)
{
    static thread_local lru_memory_cache<unique_identifier, std::shared_ptr<PIX>> pix_cache;
    
    return pix_cache.get_or_create(data.id(),
        [data](const unique_identifier& key)
        {
            std::lock_guard<std::mutex> lock { tesseract_libtiff_mutex };
            leptonica_stderr_capturer leptonica_stderr_capturer;
            std::optional<std::filesystem::path> path = data.path();
            PIX* pix;
            if (path)
            {
                pix = pixRead(path->string().c_str());
            }
            else
            {
                std::span<const std::byte> pic_data = data.span();
                pix = pixReadMem((const unsigned char*)(pic_data.data()), pic_data.size());
            }
            throw_if(!pix, "Could not load image", errors::uninterpretable_data{}, leptonica_stderr_capturer.contents());
            return std::shared_ptr<PIX>{pix, [](PIX* pix) { pixDestroy(&pix); }};
        });
}

ocr_data_path default_tessdata_path()
{
    std::filesystem::path def_tessdata_path = resource_path("tessdata-fast").string();
    throw_if (!std::filesystem::exists(def_tessdata_path),
        "Could not find tessdata in default location", def_tessdata_path, errors::program_corrupted{});
    return ocr_data_path{def_tessdata_path};
}

const std::vector<mime_type> supported_mime_types
{
    mime_type{"image/tiff"},
    mime_type{"image/jpeg"},
    mime_type{"image/bmp"},
    mime_type{"image/x-ms-bmp"},
    mime_type{"image/png"},
    mime_type{"image/x-portable-anymap"},
    mime_type{"image/webp"}
};

} // anonymous namespace

OCRParser::OCRParser(const std::vector<Language>& languages,
                     ocr_confidence_threshold ocr_confidence_threshold_arg,
                     ocr_timeout ocr_timeout_arg,
                     ocr_data_path ocr_data_path_arg)
{
    impl().m_languages = languages;
    impl().m_ocr_confidence_threshold = ocr_confidence_threshold_arg;
    impl().m_ocr_timeout = ocr_timeout_arg;
    impl().m_ocr_data_path = ocr_data_path_arg.v.empty() ? default_tessdata_path() : ocr_data_path_arg;
}

void OCRParser::parse(const data_source& data, const std::vector<Language>& languages)
{
    tessAPIWrapper api{ nullptr, tessAPIDeleter };
    try
    {
        api.reset(new TessBaseAPI{});
    }
    catch(const std::bad_alloc& exc)
    {
        throw;
    }

    std::string langs = std::accumulate(languages.begin(), languages.end(), std::string{},
      [](const std::string& acc, const Language& lang)
      {
        return acc + (acc.empty() ? "" : "+") + boost::lexical_cast<std::string>(lang);
      });
    docwire_log_var(langs);

    {
        std::lock_guard<std::mutex> tesseract_libtiff_mutex_lock{ tesseract_libtiff_mutex };
        throw_if (api->Init(impl().m_ocr_data_path.v.string().c_str(), langs.c_str()) != 0,
            "Could not initialize tesseract", impl().m_ocr_data_path.v.string(), langs);
    }

    // Read the image and convert to a gray-scale image
    pix_unique_ptr gray{ nullptr };

    std::shared_ptr<PIX> image = load_pix(data);

    pix_unique_ptr inverted{ nullptr };
    try
    {
        gray.reset(pixToGrayscale(image.get()));
        std::unique_ptr<NUMA, decltype([](NUMA* numa) { numaDestroy(&numa); })> histogram{ pixGetGrayHistogram(gray.get(), 1) };

        double weight_sum{ 0 };
        double sum{ 0 };

        constexpr int shadeScale{ 256 }; // where 0 - black, 255 - white
        for(int i{ 0 }; i < shadeScale; ++i)
        {
            // calculating weighted average of shade
            sum += (i+1) * histogram->array[i];
            weight_sum += histogram->array[i];
        }

        if(static_cast<int>(sum / weight_sum) <= shadeScale / 2)
        {
            inverted.reset(pixInvert(nullptr, gray.get()));
        }
        else
        {
            inverted.reset(gray.release());
        }
    }
    catch(const std::exception& e)
    {
        throw;
    }

    api->SetImage(inverted.get());
    tesseract::ETEXT_DESC monitor;
    if (impl().m_ocr_timeout.v)
    {
        monitor.set_deadline_msecs(*impl().m_ocr_timeout.v);
    }
    monitor.cancel = &pimpl_impl<OCRParser>::cancel;
    monitor.cancel_this = reinterpret_cast<void*>(&impl().m_context_stack.top());

    // Recognize the image
    api->Recognize(&monitor);

    // Iterate through results and emit messages: Block -> Paragraph -> Line -> Word
    const float confidence_threshold = impl().m_ocr_confidence_threshold.v.value_or(75.0f);

    std::unique_ptr<tesseract::ResultIterator> rit(api->GetIterator());
    if (!rit) {
        docwire_log(error) << "Tesseract GetIterator() returned null.";
        return;
    }

    rit->Begin(); // Start at page level
    do { // Iterate Blocks (RIL_BLOCK)
        // TODO: Add styling attributes from rit->BoundingBox(RIL_BLOCK, ...) if needed
        impl().emit_message(document::Section{});

        do { // Iterate Paragraphs (RIL_PARA) within the current Block
            // TODO: Add styling attributes from rit->BoundingBox(RIL_PARA, ...) if needed
            impl().emit_message(document::Paragraph{});
            bool current_line_had_high_confidence_text = false; // Used for BreakLine logic

            do { // Iterate TextLines (RIL_TEXTLINE) within the current Paragraph
                current_line_had_high_confidence_text = false; // Reset for each new line
                bool previous_word_on_line_was_high_confidence = false; // For spacing between words

                do { // Iterate Words (RIL_WORD) within the current TextLine
                    const char* word_chars = rit->GetUTF8Text(tesseract::RIL_WORD);
                    std::string current_word_str;
                    if (word_chars)
                    {
                        current_word_str = word_chars;
                        delete[] word_chars; // Tesseract requires freeing this
                        boost::algorithm::trim(current_word_str);
                    }
                    if (!current_word_str.empty()) {
                        float conf = rit->Confidence(tesseract::RIL_WORD);
                        if (conf >= confidence_threshold) {
                            if (previous_word_on_line_was_high_confidence) {
                                impl().emit_message(document::Text{" "}); // Add space before the current word
                            }
                            impl().emit_message(document::Text{current_word_str});
                            current_line_had_high_confidence_text = true;
                            previous_word_on_line_was_high_confidence = true;
                        } else {
                            previous_word_on_line_was_high_confidence = false; // Reset if low-confidence word encountered
                        }
                    } else { // Word was null (word_chars == nullptr) or became empty after trim
                        // This represents a break in the flow of actual text words,
                        // so reset the flag to prevent a space before the next actual word.
                        previous_word_on_line_was_high_confidence = false;
                    }

                    // Regardless of word content, if the iterator is at the last word
                    // of the current structural RIL_TEXTLINE, break this word loop.
                    if (rit->IsAtFinalElement(tesseract::RIL_TEXTLINE, tesseract::RIL_WORD)) {
                        break;
                    }
                } while (rit->Next(tesseract::RIL_WORD)); // Attempt to advance to the next word

                // End of TextLine processing
                if (current_line_had_high_confidence_text) {
                    // Add BreakLine if not the last line of the current paragraph
                    if (!rit->IsAtFinalElement(tesseract::RIL_PARA, tesseract::RIL_TEXTLINE)) {
                        // TODO: Add styling attributes from rit->BoundingBox(RIL_TEXTLINE, ...) to BreakLine if needed
                        impl().emit_message(document::BreakLine{});
                    }
                }
                // Check if this was the last line in the current paragraph before trying to advance to the next line.
                if (rit->IsAtFinalElement(tesseract::RIL_PARA, tesseract::RIL_TEXTLINE)) {
                    break; // Break from RIL_TEXTLINE loop; Next(RIL_PARA) will be called.
                }
            } while (rit->Next(tesseract::RIL_TEXTLINE)); // Advances to next line in this paragraph

            // End of Paragraph processing
            impl().emit_message(document::CloseParagraph{});
            // Check if this was the last paragraph in the current block before trying to advance to the next paragraph.
            if (rit->IsAtFinalElement(tesseract::RIL_BLOCK, tesseract::RIL_PARA)) {
                break; // Break from RIL_PARA loop; Next(RIL_BLOCK) will be called.
            }
        } while (rit->Next(tesseract::RIL_PARA)); // Advances to next paragraph in this block

        // End of Block processing
        impl().emit_message(document::CloseSection{});
    } while (rit->Next(tesseract::RIL_BLOCK)); // Advances to next block on the page
}

continuation OCRParser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
    auto process = [this](const data_source& data, const message_callbacks& emit_message) {
        docwire_log(debug) << "Using OCR parser.";
        scoped::stack_push<context> context_guard{impl().m_context_stack, context{emit_message}};
        emit_message(document::Document{.metadata = []() { return attributes::Metadata{}; }});
        parse(data, impl().m_languages.size() > 0 ? impl().m_languages : std::vector({ Language::eng }));
        emit_message(document::CloseDocument{});
        return continuation::proceed;
    };

    if (msg->is<data_source>())
    {
        const data_source& data = msg->get<data_source>();
        data.assert_not_encrypted();
        if (data.has_highest_confidence_mime_type_in(supported_mime_types))
            return process(msg->get<data_source>(), emit_message);
        else
            return emit_message(std::move(msg));
    }
    else if (msg->is<document::Image>())
    {
        document::Image& image = msg->get<document::Image>();
        image.source.assert_not_encrypted();
        if (!image.source.highest_confidence_mime_type().has_value())
            return emit_message(std::move(msg));
        if (!image.source.has_highest_confidence_mime_type_in(supported_mime_types))
            return emit_message(std::move(msg));
        docwire_log(debug) << "OCRParser: Setting up streamer for document::Image.";
        image.structured_content_streamer =
            [process, data = image.source](const message_callbacks& emit_message) -> continuation
            {
                try
                {
                    return process(data, emit_message);
                }
                catch (const std::exception&)
                {
                    emit_message(make_nested_ptr(std::current_exception(), make_error("OCR processing of image failed")));
                    return continuation::proceed;
                }
            };
        return emit_message(std::move(msg));
    }
    else
        return emit_message(std::move(msg));
}

} // namespace docwire
