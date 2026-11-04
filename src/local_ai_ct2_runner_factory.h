/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_CT2_RUNNER_FACTORY_H
#define DF0_LOCAL_AI_CT2_RUNNER_FACTORY_H

#include "ai_runner.h"
#include "ct2_runner.h"
#include "resource_path.h"
#include "audit/auditor_ref.hpp"
#include "audit/core.hpp"

#include <concepts>
#include <memory>

namespace df0::ai::local::ct2
{

template<typename auditor_type>
inline std::shared_ptr<df0::ai::ai_runner> make_default_runner(
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::ct2::ct2_runner<auditor_type>>(
        resource_path("flan-t5-large-ct2-int8"),
        auditor);
}

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
inline std::shared_ptr<df0::ai::ai_runner> make_default_runner()
{
    return make_default_runner<stateless_auditor_type>(
        df0::audit::auditor_ref<stateless_auditor_type>{});
}

template<typename auditor_type>
inline std::shared_ptr<df0::ai::ai_runner> make_embedding_runner(
    df0::audit::auditor_ref<auditor_type> auditor)
{
    return std::make_shared<df0::ai::ct2::ct2_runner<auditor_type>>(
        resource_path("multilingual-e5-small-ct2-int8"),
        auditor);
}

template<audit::stateless_auditor stateless_auditor_type = audit::null_auditor>
inline std::shared_ptr<df0::ai::ai_runner> make_embedding_runner()
{
    return make_embedding_runner<stateless_auditor_type>(
        df0::audit::auditor_ref<stateless_auditor_type>{});
}

} // namespace df0::ai::local::ct2

#endif
