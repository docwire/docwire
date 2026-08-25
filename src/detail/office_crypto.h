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

#ifndef DOCWIRE_DETAIL_OFFICE_CRYPTO_H
#define DOCWIRE_DETAIL_OFFICE_CRYPTO_H

#include "data_source.h"
#include "serialization_data_source.h"
#include "log_scope.h"
#include "detail/thread_safe_ole_storage.h"
#include <vector>

namespace docwire
{

inline bool is_encrypted_with_ms_offcrypto(const data_source& data)
{
	DOCWIRE_LOG_SCOPE(data);
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

} // namespace docwire

#endif // DOCWIRE_DETAIL_OFFICE_CRYPTO_H
