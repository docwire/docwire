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
    const char* file_name_ptr = location.file_name();
    const char* function_name_ptr = location.function_name();

    std::string filename = file_name_ptr
        ? std::filesystem::path(file_name_ptr).filename().string()
        : std::string{};

    std::string funcname = function_name_ptr
        ? docwire::type_name::pretty_function(function_name_ptr)
        : std::string{};

    auto matches_file = [&](std::string_view pattern)
    {
        return wildcard_match(pattern, filename);
    };

    auto matches_func = [&](std::string_view pattern)
    {
        return wildcard_match(pattern, funcname);
    };

    auto matches_tag = [&](std::string_view pattern)
    {
        for (std::size_t i = 0; i < entry_tags.size(); ++i)
        {
            if (wildcard_match(pattern, entry_tags[i]))
                return true;
        }
        return false;
    };

    auto matches_one = [&](const filter_rule& rule)
    {
        if (rule.type == filter_rule::FILE)
            return matches_file(rule.value);
        if (rule.type == filter_rule::FUNC)
            return matches_func(rule.value);
        if (rule.type == filter_rule::TAG)
            return matches_tag(rule.value);
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
