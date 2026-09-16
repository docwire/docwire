#ifndef DOCWIRE_AUDIT_AUDIT_TAG_H
#define DOCWIRE_AUDIT_AUDIT_TAG_H

#include "diagnostic_context.h"

#include <tuple>

namespace docwire::audit
{

template<typename T>
concept audit_tag = docwire::context_tag<T>;

} // namespace docwire::audit

#endif
