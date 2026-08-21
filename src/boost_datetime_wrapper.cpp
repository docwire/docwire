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

#include "boost_datetime_wrapper.h"

#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iomanip>
#include <sstream>
#include <string>

namespace docwire::detail
{

std::string local_iso_datetime_with_timezone_offset()
{
    namespace bpt = boost::posix_time;

    const bpt::ptime utc_time = bpt::second_clock::universal_time();

    using local_adjustor = boost::date_time::c_local_adjustor<bpt::ptime>;
    const bpt::ptime local_time = local_adjustor::utc_to_local(utc_time);

    const bpt::time_duration offset = local_time - utc_time;
    const long offset_seconds = offset.total_seconds();
    const int offset_hours = static_cast<int>(offset_seconds / 3600);
    const int offset_minutes = static_cast<int>((offset_seconds % 3600) / 60);

    std::ostringstream stream;
    stream << bpt::to_iso_extended_string(local_time)
           << std::showpos
           << std::setw(4)
           << std::setfill('0')
           << (offset_hours * 100 + offset_minutes);

    return stream.str();
}

} // namespace docwire::detail
