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

#include "convert_chrono.h"

#include "convert_numeric.h" // IWYU pragma: keep
#include "log_scope.h"

namespace docwire::convert
{

namespace
{

bool try_to_int(std::string_view sv, int& out) noexcept
{
    if (auto val = try_to<int>(sv))
    {
        out = *val;
        return true;
    }
    return false;
}

std::optional<std::chrono::sys_seconds> create_sys_seconds(int year, int month, int day, int hour, int minute, int second) noexcept
{
    using namespace std::chrono;
    
    const year_month_day ymd{std::chrono::year{year}, std::chrono::month{static_cast<unsigned>(month)}, std::chrono::day{static_cast<unsigned>(day)}};
    if (!ymd.ok())
        return std::nullopt;

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 60)
        return std::nullopt;

    return sys_days{ymd} + hours{hour} + minutes{minute} + seconds{second};
}

std::optional<std::chrono::sys_seconds> parse_iso8601(std::string_view s) noexcept
{
    int y, m, d, h, min, sec;
    // ISO 8601: YYYY-MM-DDTHH:MM:SS
    if (s.length() >= 19 &&
        try_to_int(s.substr(0, 4), y) && s[4] == '-' &&
        try_to_int(s.substr(5, 2), m) && s[7] == '-' &&
        try_to_int(s.substr(8, 2), d) && s[10] == 'T' &&
        try_to_int(s.substr(11, 2), h) && s[13] == ':' &&
        try_to_int(s.substr(14, 2), min) && s[16] == ':' &&
        try_to_int(s.substr(17, 2), sec))
        return create_sys_seconds(y, m, d, h, min, sec);
    return std::nullopt;
}

std::optional<std::chrono::sys_seconds> parse_openoffice_legacy(std::string_view s) noexcept
{
    int y, m, d, h, min, sec;
    // OpenOffice legacy: YYYYMMDD;HHMMSSff (fractional seconds 'ff' are ignored)
    if (s.length() >= 17 &&
        try_to_int(s.substr(0, 4), y) &&
        try_to_int(s.substr(4, 2), m) &&
        try_to_int(s.substr(6, 2), d) && s[8] == ';' &&
        try_to_int(s.substr(9, 2), h) &&
        try_to_int(s.substr(11, 2), min) &&
        try_to_int(s.substr(13, 2), sec))
        return create_sys_seconds(y, m, d, h, min, sec);
    return std::nullopt;
}

std::optional<std::chrono::sys_seconds> parse_asn1(std::string_view s) noexcept
{
    int y, m, d, h, min, sec;
    // ASN.1 format: YYYYMMDDHHmmSS
    if (s.length() >= 14 &&
        try_to_int(s.substr(0, 4), y) &&
        try_to_int(s.substr(4, 2), m) &&
        try_to_int(s.substr(6, 2), d) &&
        try_to_int(s.substr(8, 2), h) &&
        try_to_int(s.substr(10, 2), min) &&
        try_to_int(s.substr(12, 2), sec))
        return create_sys_seconds(y, m, d, h, min, sec);
    return std::nullopt;
}

} // anonymous namespace

std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::iso8601 s, dest_type_tag<std::chrono::sys_seconds>) noexcept
{
    log_scope(s);
    return parse_iso8601(s.v);
}

std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::openoffice_legacy s, dest_type_tag<std::chrono::sys_seconds>) noexcept
{
    log_scope(s);
    return parse_openoffice_legacy(s.v);
}

std::optional<std::chrono::sys_seconds> convert_impl(with::date_format::asn1 s, dest_type_tag<std::chrono::sys_seconds>) noexcept
{
    log_scope(s);
    return parse_asn1(s.v);
}

std::optional<std::string> convert_impl(std::chrono::sys_seconds tp, dest_type_tag<std::string>) noexcept
{
    auto ymd = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(tp)};
    auto time = std::chrono::hh_mm_ss{tp - std::chrono::floor<std::chrono::days>(tp)};

    auto to_string_padded = [](int value) {
        return (value < 10 ? "0" : "") + std::to_string(value);
    };

    return std::to_string(static_cast<int>(ymd.year())) + '-' +
           to_string_padded(static_cast<unsigned>(ymd.month())) + '-' +
           to_string_padded(static_cast<unsigned>(ymd.day())) + ' ' +
           to_string_padded(time.hours().count()) + ':' +
           to_string_padded(time.minutes().count()) + ':' +
           to_string_padded(time.seconds().count());
}

} // namespace docwire::convert
