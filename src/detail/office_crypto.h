/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_DETAIL_OFFICE_CRYPTO_H
#define DF0_DETAIL_OFFICE_CRYPTO_H

#include "data_source.h"
#include "serialization_data_source.h"
#include "detail/thread_safe_ole_storage.h"
#include <vector>

namespace df0
{

inline bool is_encrypted_with_ms_offcrypto(const data_source& data)
{
	thread_safe_ole_storage storage(data.span());
	if (storage.isValid())
	{
		std::vector<std::string> dirs;
		if (storage.getStreamsAndStoragesList(dirs))
		{
			for (size_t i = 0; i < dirs.size(); ++i)
			{
				if (dirs[i] == "\006DataSpaces")
					return true;
			}
		}
	}
	return false;
}

} // namespace df0

#endif // DF0_DETAIL_OFFICE_CRYPTO_H
