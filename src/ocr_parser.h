/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++17/20. SourceForge Community Choice & Microsoft support. AI-driven processing.   */
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

#include "formatting_style.h"
#include "language.h"
#include "parser.h"
#include "parser_builder.h"

namespace docwire
{

class OCRParser : public Parser
{
private:
    struct Implementation;
    struct ImplementationDeleter { void operator() (Implementation*); };
    std::unique_ptr<Implementation, ImplementationDeleter> impl;
    const unsigned int TIMEOUT = 30000;

public:
    static std::string get_default_tessdata_prefix();

    OCRParser(const OCRParser& ocr_parser);
    OCRParser(const std::string& file_name, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
    OCRParser(const char* buffer, size_t size, const std::shared_ptr<ParserManager> &inParserManager = nullptr);
    ~OCRParser();

    void parse() const override;
    Parser& addOnNewNodeCallback(NewNodeCallback callback) override;
    static std::vector <std::string> getExtensions() {return {"tiff", "jpeg", "bmp", "png", "pnm", "jfif", "jpg", "webp"};}
    Parser& withParameters(const ParserParameters &parameters) override;

    void setTessdataPrefix(const std::string& tessdata_prefix);
    bool isOCR() const;
private:
    std::string plainText(const FormattingStyle& formatting, const std::vector<Language>& languages) const;
};

} // namespace docwire

#endif // DOCWIRE_OCR_PARSER_H
