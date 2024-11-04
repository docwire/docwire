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

#include "parser_parameters.h"

#include <leptonica/allheaders.h>
#include <leptonica/array_internal.h>
#include <leptonica/pix_internal.h>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

#include <boost/lexical_cast.hpp>
#include <boost/signals2.hpp>

#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <magic_enum_iostream.hpp>
#include "log.h"
#include "lru_memory_cache.h"
#include <mutex>
#include <numeric>
#include "resource_path.h"

namespace docwire
{

struct OCRParser::Implementation
{
    OCRParser* m_owner;
    std::string m_tessdata_prefix;

    Implementation(OCRParser* owner)
        : m_owner(owner)
    {}

    static bool cancel (void* data, int words)
    {
        auto impl = reinterpret_cast<OCRParser::Implementation*>(data);
        Info info{tag::PleaseWait{}};
        impl->m_owner->sendTag(info);
        return info.cancel;
    }
};  

std::string OCRParser::get_default_tessdata_prefix()
{
    return "./tessdata/";
}

OCRParser::OCRParser()
    : impl(std::make_unique<Implementation>(this))
{
}

OCRParser::~OCRParser() = default;

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
        throw make_error("Format not supported", pix->d);
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
            std::optional<std::filesystem::path> path = data.path();
            if (path)
            {
                return std::shared_ptr<PIX>{
                    pixRead(path->string().c_str()),
                    [](PIX* pix) { pixDestroy(&pix); }
                };
            }
            else
            {
                std::span<const std::byte> pic_data = data.span();
                return std::shared_ptr<PIX>{
                    pixReadMem((const unsigned char*)(pic_data.data()), pic_data.size()),
                    [](PIX* pix) { pixDestroy(&pix); }
                };
            }
        });
}

} // anonymous namespace

std::string OCRParser::parse(const data_source& data, const std::vector<Language>& languages) const
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

    auto tess_data_prefix = m_parameters.getParameterValue<std::string>("TESSDATA_PREFIX");
    if (tess_data_prefix)
    {
      impl->m_tessdata_prefix = *tess_data_prefix;
    }
    else
    {
      impl->m_tessdata_prefix = resource_path("tessdata-fast").string();
    }

    std::string langs = std::accumulate(languages.begin(), languages.end(), std::string{},
      [](const std::string& acc, const Language& lang)
      {
        return acc + (acc.empty() ? "" : "+") + boost::lexical_cast<std::string>(lang);
      });
    docwire_log_var(langs);

    {
        std::lock_guard<std::mutex> tesseract_libtiff_mutex_lock{ tesseract_libtiff_mutex };
        throw_if (api->Init(impl->m_tessdata_prefix.c_str(), langs.c_str()) != 0, "Could not initialize tesseract", impl->m_tessdata_prefix, langs);
    }

    // Read the image and convert to a gray-scale image
    pix_unique_ptr gray{ nullptr };

    std::shared_ptr<PIX> image = load_pix(data);
    throw_if (image == nullptr, "Could not load image");

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
    auto ocr_timeout = m_parameters.getParameterValue<int32_t>("ocr_timeout");
    if (ocr_timeout)
    {
        monitor.set_deadline_msecs(*ocr_timeout);
    }
    monitor.cancel = &Implementation::cancel;
    monitor.cancel_this = reinterpret_cast<void*>(impl.get());
    api->Recognize(&monitor);
    auto txt = api->GetUTF8Text();
    std::string output{ txt };
    delete[] txt;
    return output;
}

void OCRParser::setTessdataPrefix(const std::string& tessdata_prefix)
{
    impl->m_tessdata_prefix = tessdata_prefix;
}

Parser&
OCRParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
    return *this;
}

void OCRParser::parse(const data_source& data) const
{
  docwire_log(debug) << "Using OCR parser.";
  auto language = m_parameters.getParameterValue<std::vector<Language>>("languages");
  sendTag(tag::Document{.metadata = []() { return attributes::Metadata{}; }});
  std::string plain_text = parse(data, language && language->size() > 0 ? *language : std::vector({ Language::eng }));
  sendTag(tag::Text{.text = plain_text});
  sendTag(tag::CloseDocument{});
}

} // namespace docwire
