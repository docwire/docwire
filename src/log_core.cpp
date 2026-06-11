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

#include "log_core.h"

#include <boost/algorithm/string.hpp>
#include <boost/core/demangle.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include "serialization_filesystem.h" // IWYU pragma: keep
#include "serialization_thread_id.h" // IWYU pragma: keep
#include <mutex>
#include <sstream>
#include "type_name_base.h"

namespace docwire::log
{

struct filter_rule
{
	enum { TAG, FILE, FUNC } type;
	std::string value;
	bool is_negative;
};

struct filter_spec
{
	std::vector<filter_rule> rules;
	bool wildcard_enabled = false;
};

static filter_spec parse_log_filter(const std::string& filter_str)
{
	filter_spec filter;
	std::vector<std::string> rules_str;
	boost::split(rules_str, filter_str, boost::is_any_of(",; "));
	for (auto& rule_str : rules_str)
	{
		boost::trim(rule_str);
		if (rule_str.empty()) continue;

		if (rule_str == "*")
		{
			filter.wildcard_enabled = true;
			continue;
		}

		filter_rule rule;
		rule.is_negative = (rule_str.front() == '-');
		std::string_view rule_view = rule_str;
		if (rule.is_negative)
			rule_view.remove_prefix(1);

		if (rule_view.starts_with("@file:"))
		{
			rule.type = filter_rule::FILE;
			rule.value = std::string(rule_view.substr(6));
		}
		else if (rule_view.starts_with("@func:"))
		{
			rule.type = filter_rule::FUNC;
			rule.value = std::string(rule_view.substr(6));
		}
		else
		{
			rule.type = filter_rule::TAG;
			rule.value = std::string(rule_view);
		}
		filter.rules.push_back(std::move(rule));
	}
	return filter;
}

static std::mutex g_log_filter_mutex;

static filter_spec g_log_filter;
static std::string g_log_filter_str;

void set_filter(const std::string& filter_spec)
{
	std::lock_guard lock(g_log_filter_mutex);
	g_log_filter = parse_log_filter(filter_spec);
	g_log_filter_str = filter_spec;
}

std::string get_filter()
{
	std::lock_guard lock(g_log_filter_mutex);
	return g_log_filter_str;
}

static bool wildcard_match(const std::string_view& pattern, const std::string_view& text)
{
	if (pattern == "*") return true;

	auto pattern_iter = pattern.begin();
	auto text_iter = text.begin();
	auto last_star_pattern_iter = pattern.end();
	auto last_star_text_iter = text.end();

	while (text_iter != text.end())
	{
		if (pattern_iter != pattern.end() && *pattern_iter == '*')
		{
			// Star found, save its position and the current text position for backtracking.
			last_star_pattern_iter = pattern_iter++;
			last_star_text_iter = text_iter;
		}
		else if (pattern_iter != pattern.end() && (*pattern_iter == '?' || *pattern_iter == *text_iter))
		{
			++pattern_iter;
			++text_iter;
		}
		else if (last_star_pattern_iter != pattern.end())
		{
			// Mismatch, but we have a star to backtrack to.
			// Reset pattern to after the star and advance text from the star's position.
			pattern_iter = last_star_pattern_iter + 1;
			text_iter = ++last_star_text_iter;
		}
		else
            return false;
	}

	// Consume any trailing stars in the pattern.
	while (pattern_iter != pattern.end() && *pattern_iter == '*')
		++pattern_iter;

	return pattern_iter == pattern.end();
}

bool detail::is_enabled(const source_location& location, std::span<const std::string_view> tags)
{
	std::lock_guard lock(g_log_filter_mutex);
	const auto& filter = g_log_filter;

	std::string filename = std::filesystem::path(location.file_name()).filename().string();
	std::string funcname = docwire::type_name::pretty_function(location.function_name());

	// 1. Process "deny" rules first. A single negative match immediately disables the log.
	for (const auto& rule : filter.rules)
	{
		if (!rule.is_negative)
			continue;

		if ((rule.type == filter_rule::FILE && wildcard_match(rule.value, filename)) ||
			(rule.type == filter_rule::FUNC && wildcard_match(rule.value, funcname)) ||
			(rule.type == filter_rule::TAG && std::any_of(tags.begin(), tags.end(), [&](auto tag) { return wildcard_match(rule.value, tag); })))
		{
			return false;
		}
	}

	// 2. If the global wildcard '*' is enabled, any log that has not been explicitly
	// denied by this point is allowed. This is a fast path.
	if (filter.wildcard_enabled)
		return true;

	// 3. Process all positive rules. The first match of any type is sufficient to enable the log.
	for (const auto& rule : filter.rules)
	{
		if (rule.is_negative)
			continue;

		if ((rule.type == filter_rule::FILE && wildcard_match(rule.value, filename)) ||
			(rule.type == filter_rule::FUNC && wildcard_match(rule.value, funcname)) ||
			(rule.type == filter_rule::TAG && std::any_of(tags.begin(), tags.end(), [&](auto tag) { return wildcard_match(rule.value, tag); })))
		{
			return true;
		}
	}

	return false; // Not enabled by any rule.
}

static std::atomic<bool> g_logging_enabled{false};
static std::function<void(const log::record&)> g_log_callback;
static std::mutex g_log_callback_mutex;

static void write_log_record(const log::record& rec)
{
	std::lock_guard lock(g_log_callback_mutex);
	if (g_log_callback)
		g_log_callback(rec);
}

void set_sink(std::function<void(const log::record&)> callback)
{
	std::lock_guard lock(g_log_callback_mutex);
	g_log_callback = std::move(callback);
	g_logging_enabled.store(static_cast<bool>(g_log_callback), std::memory_order_release);
}

std::function<void(const record&)> get_sink()
{
    std::lock_guard lock(g_log_callback_mutex);
    return g_log_callback;
}

bool detail::is_logging_enabled()
{
	return g_logging_enabled.load(std::memory_order_acquire);
}

serialization::object create_base_metadata(source_location location)
{
	serialization::object metadata;
	boost::posix_time::ptime utc_time = boost::posix_time::second_clock::universal_time();
	boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adjustor;
	boost::posix_time::ptime local_time = local_adjustor.utc_to_local(utc_time);
	boost::posix_time::time_duration timezone_offset = local_time - utc_time;
	long timezone_offset_seconds = timezone_offset.total_seconds();
	int timezone_offset_hours = timezone_offset_seconds / 3600;
	int timezone_offset_minutes = (timezone_offset_seconds % 3600) / 60;
	std::stringstream time_stream;
	time_stream << boost::posix_time::to_iso_extended_string(local_time) << std::setw(5) << std::setfill('0')
				<< std::internal << std::showpos << timezone_offset_hours * 100 + timezone_offset_minutes;

	metadata.v = {
		{"timestamp", time_stream.str()},
		{"file", serialization::full(std::filesystem::path(location.file_name()).filename())},
		{"line", static_cast<std::int64_t>(location.line())},
		{"function", docwire::type_name::pretty_function(location.function_name())},
		{"thread_id", serialization::full(std::this_thread::get_id())}
	};
	return metadata;
}

record::record(source_location location, serialization::array&& context)
	: m_location(location), m_context(std::move(context))
{}

record::~record()
{
	try
	{
		write_log_record(*this);
	} catch(...) {
		// Don't let exceptions escape from a destructor
	}
}

} // namespace docwire::log
