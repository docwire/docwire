/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_LOG_FILTER_RULES_H
#define DOCWIRE_LOG_FILTER_RULES_H

#include <string>
#include <string_view>
#include <vector>

namespace docwire::log::detail
{

struct filter_rule
{
    enum
    {
        FILE,
        FUNC,
        TAG
    };

    int type{TAG};
    std::string value;
    bool is_negative{false};
};

struct filter_spec
{
    std::vector<filter_rule> rules;
    bool wildcard_enabled{false};
};

inline bool wildcard_match(std::string_view pattern, std::string_view text)
{
    size_t p = 0;
    size_t t = 0;
    size_t star = std::string_view::npos;
    size_t match = 0;

    while (t < text.size())
    {
        if (p < pattern.size() && (pattern[p] == text[t] || pattern[p] == '?'))
        {
            ++p;
            ++t;
        }
        else if (p < pattern.size() && pattern[p] == '*')
        {
            star = p++;
            match = t;
        }
        else if (star != std::string_view::npos)
        {
            p = star + 1;
            t = ++match;
        }
        else
        {
            return false;
        }
    }

    while (p < pattern.size() && pattern[p] == '*')
    {
        ++p;
    }

    return p == pattern.size();
}

inline filter_spec parse_log_filter(const std::string& filter_spec_text)
{
    filter_spec spec;

    auto trim = [](const std::string& s)
    {
        constexpr std::string_view whitespace = " \t\r\n";
        size_t first = s.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return std::string{};
        size_t last = s.find_last_not_of(whitespace);
        return s.substr(first, last - first + 1);
    };

    std::string spec_copy = trim(filter_spec_text);
    if (spec_copy.empty())
        return spec;

    if (spec_copy == "*")
    {
        spec.wildcard_enabled = true;
        return spec;
    }

    size_t start = 0;
    while (start <= spec_copy.size())
    {
        size_t comma = spec_copy.find(',', start);
        std::string raw = trim(spec_copy.substr(start, comma == std::string::npos ? std::string::npos : comma - start));

        if (!raw.empty())
        {
            filter_rule rule;
            rule.is_negative = raw.front() == '-';

            if (rule.is_negative)
                raw = trim(raw.substr(1));

            if (!raw.empty() && raw.front() == '@')
                raw = trim(raw.substr(1));

            if (raw.rfind("file:", 0) == 0)
            {
                rule.type = filter_rule::FILE;
                rule.value = raw.substr(5);
            }
            else if (raw.rfind("func:", 0) == 0)
            {
                rule.type = filter_rule::FUNC;
                rule.value = raw.substr(5);
            }
            else
            {
                rule.type = filter_rule::TAG;
                rule.value = raw;
            }

            spec.rules.push_back(rule);
        }

        if (comma == std::string::npos)
            break;

        start = comma + 1;
    }

    return spec;
}

} // namespace docwire::log::detail

#endif // DOCWIRE_LOG_FILTER_RULES_H
