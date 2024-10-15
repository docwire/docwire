/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: GPL-2.0-only OR LicenseRef-DocWire-Commercial                                                                   */
/*********************************************************************************************************************************************/

#ifndef DOCWIRE_THROW_IF_H
#define DOCWIRE_THROW_IF_H

#include "make_error.h"

#define DOCWIRE_THROW_IF(triggering_expression, ...) \
	do { \
		if (triggering_expression) \
		{ \
			constexpr const char* triggering_condition = #triggering_expression; \
			throw DOCWIRE_MAKE_ERROR(triggering_condition __VA_OPT__(,) __VA_ARGS__); \
		} \
	} while(0)

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
	#define throw_if DOCWIRE_THROW_IF
#endif

#endif
