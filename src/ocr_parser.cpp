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

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/signals2.hpp>

#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <magic_enum_iostream.hpp>
#include "exception.h"
#include "log.h"
#include "misc.h"
#include <mutex>
#include <numeric>

namespace docwire
{

struct OCRParser::Implementation
{
    explicit Implementation(const std::string& file_name)
    : m_file_name{ file_name }, m_buffer{nullptr}, m_buffer_size{0}
    { 
    }

  Implementation(const char* buffer, size_t size)
    : m_buffer{buffer}, m_buffer_size{size}
  {
  }

    ~Implementation() = default;

    std::string m_file_name{};
    const char* m_buffer;
    size_t m_buffer_size;
    std::string m_tessdata_prefix;
    boost::signals2::signal<void(Info &info)> m_on_new_node_signal;
};  

void OCRParser::ImplementationDeleter::operator() (Implementation* impl)
{
    delete impl;
}

std::string OCRParser::get_default_tessdata_prefix()
{
    return "./tessdata/";
}

OCRParser::OCRParser(const OCRParser& ocr_parser)
{
  if (!ocr_parser.impl->m_file_name.empty())
  {
    impl = std::unique_ptr<Implementation, ImplementationDeleter>
      {new Implementation{ ocr_parser.impl->m_file_name }, ImplementationDeleter{}};
  }
  else
  {
    impl = std::unique_ptr<Implementation, ImplementationDeleter>
      {new Implementation{ocr_parser.impl->m_buffer, ocr_parser.impl->m_buffer_size}, ImplementationDeleter{}};
  }
}

OCRParser::OCRParser(const std::string& file_name, const Importer* inImporter)
: Parser(inImporter)
{
  impl = std::unique_ptr<Implementation, ImplementationDeleter>{new Implementation{file_name}, ImplementationDeleter{}};
}

OCRParser::OCRParser(const char* buffer, size_t size, const Importer* inImporter)
: Parser(inImporter)
{
  impl = std::unique_ptr<Implementation, ImplementationDeleter> {new Implementation{buffer, size}, ImplementationDeleter{}};
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
        throw std::runtime_error{ "Format not supported: bpp" + pix->d };
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

    auto pixDeleter = [](Pix* pix)
    {
        pixDestroy(&pix);
    };
    using PixWrapper = std::unique_ptr<Pix, decltype(pixDeleter)>;
}

bool cancel (void* data, int words)
{
  auto* signal = reinterpret_cast<boost::signals2::signal<void(Info &info)>*>(data);
  Info info;
  (*signal)(info);
  return info.cancel;
}

using magic_enum::ostream_operators::operator<<;

namespace
{
    std::mutex tesseract_libtiff_mutex;
} // anonymous namespace

std::string OCRParser::plainText(const FormattingStyle& formatting, const std::vector<Language>& languages) const
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
      impl->m_tessdata_prefix = locate_resource("tessdata-fast").string();
    }

    std::string langs = std::accumulate(languages.begin(), languages.end(), std::string{},
      [](const std::string& acc, const Language& lang)
      {
        return acc + (acc.empty() ? "" : "+") + boost::lexical_cast<std::string>(lang);
      });
    docwire_log_var(langs);

    tesseract_libtiff_mutex.lock();
    if (api->Init(impl->m_tessdata_prefix.c_str(), langs.c_str())) {
        tesseract_libtiff_mutex.unlock();
        throw RuntimeError{ "Could not initialize Tesseract." };
    }
    tesseract_libtiff_mutex.unlock();

    // Read the image and convert to a gray-scale image
    PixWrapper gray{ nullptr, pixDeleter };

    tesseract_libtiff_mutex.lock();
    PixWrapper image(impl->m_file_name.empty() ? pixReadMem((const unsigned char*)(impl->m_buffer), impl->m_buffer_size)
                     : pixRead(impl->m_file_name.c_str()), pixDeleter);
    tesseract_libtiff_mutex.unlock();

    PixWrapper inverted{ nullptr, pixDeleter };
    try
    {
        gray.reset(pixToGrayscale(image.get()));
	    NUMA* histogram = pixGetGrayHistogram(gray.get(), 1);

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
    monitor.set_deadline_msecs(TIMEOUT);
    monitor.cancel = &cancel;
    monitor.cancel_this = &(impl->m_on_new_node_signal);
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

bool OCRParser::isOCR() const
{
    if(impl == nullptr) return false;
    Pix* image = impl->m_file_name.empty() ? pixReadMem((const unsigned char*)(impl->m_buffer), impl->m_buffer_size) :
                 pixRead(impl->m_file_name.c_str());
    return image != nullptr;
}

Parser&
OCRParser::withParameters(const ParserParameters &parameters)
{
	Parser::withParameters(parameters);
    return *this;
}

void
OCRParser::parse() const
{
  docwire_log(debug) << "Using OCR parser.";
  Info info(StandardTag::TAG_TEXT);
  auto language = m_parameters.getParameterValue<std::vector<Language>>("languages");
  info.plain_text = plainText(getFormattingStyle(), language && language->size() > 0 ? *language : std::vector({ Language::eng }));
  impl->m_on_new_node_signal(info);
}

Parser& OCRParser::addOnNewNodeCallback(NewNodeCallback callback)
{
  impl->m_on_new_node_signal.connect(callback);
  return *this;
}

} // namespace docwire
