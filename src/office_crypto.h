#ifndef DOCWIRE_OFFICE_CRYPTO_H
#define DOCWIRE_OFFICE_CRYPTO_H

#include "core_export.h"
#include "data_source.h"

namespace docwire
{

DOCWIRE_CORE_EXPORT bool is_encrypted_with_ms_offcrypto(const data_source& data);

} // namespace docwire

#endif
