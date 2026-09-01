#ifndef DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H
#define DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H

#include "core_export.h"
#include <cstddef>

namespace docwire::detail
{

/**
 * @brief Returns the next process-wide unique identifier value.
 *
 * @return A monotonically increasing size_t value.
 *
 * @note This function is implemented in docwire_core and exported from it.
 *       Keeping the counter here ensures identifiers remain unique across
 *       DLL boundaries.
 */
DOCWIRE_CORE_EXPORT size_t next_unique_identifier();

} // namespace docwire::detail

#endif // DOCWIRE_UNIQUE_IDENTIFIER_GLOBALS_H
