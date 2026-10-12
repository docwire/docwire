#ifndef DOCWIRE_AUDIT_DETAIL_FILTER_RULES_HPP
#define DOCWIRE_AUDIT_DETAIL_FILTER_RULES_HPP

#include "../../source_location.h"
#include "../../type_name_base.h"

#include <algorithm>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace docwire::audit::detail
{

struct filter_rule
{
    enum type_enum
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

inline filter_spec parse_filter(const std::string& filter_spec_text)
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
        std::string raw = trim(spec_copy.substr(
            start, comma == std::string::npos ? std::string::npos : comma - start));

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

inline bool matches(
    const filter_spec& spec,
    source_location location,
    std::span<const std::string_view> entry_tags)
{
    std::string filename = std::filesystem::path(location.file_name()).filename().string();
    std::string funcname = docwire::type_name::pretty_function(location.function_name());

    auto matches_one = [&](const filter_rule& rule)
    {
        if (rule.type == filter_rule::FILE)
            return wildcard_match(rule.value, filename);
        if (rule.type == filter_rule::FUNC)
            return wildcard_match(rule.value, funcname);
        if (rule.type == filter_rule::TAG)
            return std::any_of(
                entry_tags.begin(), entry_tags.end(),
                [&](std::string_view tag) { return wildcard_match(rule.value, tag); });
        return false;
    };

    for (const auto& rule : spec.rules)
    {
        if (rule.is_negative && matches_one(rule))
            return false;
    }

    if (spec.wildcard_enabled)
        return true;

    for (const auto& rule : spec.rules)
    {
        if (!rule.is_negative && matches_one(rule))
            return true;
    }

    return false;
}

} // namespace docwire::audit::detail

#endif
