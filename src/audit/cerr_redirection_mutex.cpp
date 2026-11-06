/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team               https://github.com/df0-io/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#include "stderr_redirection.hpp"

namespace df0::audit::detail
{

std::mutex& cerr_redirection_mutex()
{
    static std::mutex mutex;
    return mutex;
}

} // namespace df0::audit::detail
