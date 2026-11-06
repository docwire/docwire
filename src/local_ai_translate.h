/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_LOCAL_AI_TRANSLATE_H
#define DF0_LOCAL_AI_TRANSLATE_H

#include "ai_translate.h"

namespace df0::ai::local {

class translate
    : public df0::pipeline::transformer_element<translate>
    , public df0::ai::translate
{
public:
    explicit translate(const std::string& language);

    explicit translate(
        const std::string& language,
        std::shared_ptr<df0::ai::ai_runner> runner);
};

} // namespace df0::ai::local

#endif // DF0_LOCAL_AI_TRANSLATE_H
