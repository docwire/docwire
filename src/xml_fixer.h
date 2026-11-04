/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_XML_FIXER_H
#define DF0_XML_FIXER_H

#include "pimpl.h"
#include <string>
#include "xml_export.h"

namespace df0
{

class DF0_XML_EXPORT xml_fixer : public with_pimpl<xml_fixer>
{
	public:
		xml_fixer();
		std::string fix(const std::string& xml);
};

} // namespace df0

#endif
