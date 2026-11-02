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

#ifndef DOCWIRE_OSHARED_H
#define DOCWIRE_OSHARED_H

#include "attributes.h"
#include "audit/auditor_ref.hpp"
#include <concepts>
#include <exception>
#include <functional>
#include <string>

namespace docwire
{

class thread_safe_ole_storage;

template<typename auditor_type>
void parse_oshared_summary_info(
    thread_safe_ole_storage& storage,
    attributes::metadata& meta,
    const std::function<void(std::exception_ptr)>& non_fatal_error_handler,
    audit::auditor_ref<auditor_type> auditor);

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
void parse_oshared_summary_info(
    thread_safe_ole_storage& storage,
    attributes::metadata& meta,
    const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
    parse_oshared_summary_info<stateless_auditor_type>(
        storage,
        meta,
        non_fatal_error_handler,
        audit::auditor_ref<stateless_auditor_type>{});
}

template<typename auditor_type>
void parse_oshared_document_summary_info(
    thread_safe_ole_storage& storage,
    int& slide_count,
    audit::auditor_ref<auditor_type> auditor);

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
void parse_oshared_document_summary_info(
    thread_safe_ole_storage& storage,
    int& slide_count)
{
    parse_oshared_document_summary_info<stateless_auditor_type>(
        storage,
        slide_count,
        audit::auditor_ref<stateless_auditor_type>{});
}

template<typename auditor_type>
std::string get_codepage_from_document_summary_info(
    thread_safe_ole_storage& storage,
    audit::auditor_ref<auditor_type> auditor);

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
std::string get_codepage_from_document_summary_info(
    thread_safe_ole_storage& storage)
{
    return get_codepage_from_document_summary_info<stateless_auditor_type>(
        storage,
        audit::auditor_ref<stateless_auditor_type>{});
}

} // namespace docwire

#endif
