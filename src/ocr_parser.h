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

#ifndef DOCWIRE_OCR_PARSER_H
#define DOCWIRE_OCR_PARSER_H

#include <string>
#include <memory>
#include <iosfwd>

#include "language.h"
#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class OCRParser : public Parser
{
private:
    struct Implementation;
    std::unique_ptr<Implementation> impl;

public:
    static std::string get_default_tessdata_prefix();

    OCRParser();
    ~OCRParser();

    void parse(const data_source& data) const override;
    static std::vector <file_extension> getExtensions()
    {
        return { file_extension{".tiff"}, file_extension{".jpeg"}, file_extension{".bmp"}, file_extension{".png"}, file_extension{".pnm"}, file_extension{".jfif"}, file_extension{".jpg"}, file_extension{".webp"} };
    }
    Parser& withParameters(const ParserParameters &parameters) override;

    void setTessdataPrefix(const std::string& tessdata_prefix);
    bool understands(const data_source& data) const override;
private:
    std::string parse(const data_source& data, const std::vector<Language>& languages) const;
};

} // namespace docwire

#endif // DOCWIRE_OCR_PARSER_H
