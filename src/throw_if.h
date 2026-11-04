/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_THROW_IF_H
#define DF0_THROW_IF_H

#include "make_error.h"

#define DF0_THROW_IF(triggering_expression, ...) \
	do { \
		if (triggering_expression) \
		{ \
			const char* triggering_condition = #triggering_expression; \
			throw DF0_MAKE_ERROR(triggering_condition __VA_OPT__(,) __VA_ARGS__); \
		} \
	} while(0)

#define DF0_THROW_IF_AT_LOCATION(triggering_expression, explicit_location, ...) \
    do { \
        if (triggering_expression) { \
			const char* triggering_condition = #triggering_expression; \
            throw DF0_MAKE_ERROR_AT_LOCATION(explicit_location, triggering_condition __VA_OPT__(,) __VA_ARGS__); \
        } \
    } while(0)

#ifdef DF0_ENABLE_SHORT_MACRO_NAMES
	#define throw_if DF0_THROW_IF
#endif

#endif
