/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_OSHARED_H
#define DF0_OSHARED_H

#include "attributes.h"
#include "audit/auditor_ref.hpp"
#include <concepts>
#include <exception>
#include <functional>
#include <string>

namespace df0
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

} // namespace df0

#endif
