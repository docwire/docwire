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

#include "error_tags.h"

#include <leptonica/allheaders.h>
#include <leptonica/array_internal.h>
#include <leptonica/pix_internal.h>
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

} // anonymous namespace

template<>
struct pimpl_impl<OCRParser> : with_pimpl_owner<OCRParser>
{
    pimpl_impl(OCRParser& owner) : with_pimpl_owner{owner} {}
    std::vector<Language> m_languages;
    ocr_timeout m_ocr_timeout;
    ocr_data_path m_ocr_data_path;

    static bool cancel (void* data, int words)
    {
        auto impl = reinterpret_cast<pimpl_impl<OCRParser>*>(data);
        Info info{tag::PleaseWait{}};
        impl->owner().sendTag(info);
        return info.cancel;
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

} // anonymous namespace

OCRParser::OCRParser(const std::vector<Language>& languages, ocr_timeout ocr_timeout, ocr_data_path ocr_data_path)
{
    impl().m_languages = languages;
    impl().m_ocr_timeout = ocr_timeout;
    impl().m_ocr_data_path = ocr_data_path.v.empty() ? default_tessdata_path() : ocr_data_path;
}

std::string OCRParser::parse(const data_source& data, const std::vector<Language>& languages)
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
    monitor.cancel_this = reinterpret_cast<void*>(const_cast<pimpl_impl<OCRParser>*>(&(impl())));
    api->Recognize(&monitor);
    auto txt = api->GetUTF8Text();
    std::string output{ txt };
    delete[] txt;
    return output;
}

void OCRParser::parse(const data_source& data)
{
  docwire_log(debug) << "Using OCR parser.";
  sendTag(tag::Document{.metadata = []() { return attributes::Metadata{}; }});
  std::string plain_text = parse(data, impl().m_languages.size() > 0 ? impl().m_languages : std::vector({ Language::eng }));
  sendTag(tag::Text{.text = plain_text});
  sendTag(tag::CloseDocument{});
}

} // namespace docwire
