#include "error.h"

namespace docwire::errors
{

base::base(const source_location& location)
	: location(location)
{
}

base::base(const base&) = default;
base::base(base&&) = default;
base& base::operator=(const base&) = default;
base& base::operator=(base&&) = default;
base::~base() = default;

const char* base::what() const noexcept
{
	return typeid(*this).name();
}

} // namespace docwire::errors
