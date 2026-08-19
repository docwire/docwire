#include "office_crypto.h"

#include "serialization_data_source.h" // IWYU pragma: keep
#include "log_scope.h"
#include "detail/thread_safe_ole_storage.h"
#include <vector>

namespace docwire
{

bool is_encrypted_with_ms_offcrypto(const data_source& data)
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
