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

#ifndef DOCWIRE_MAKE_ERROR_H
#define DOCWIRE_MAKE_ERROR_H

#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include "error.h"
#include "nested_exception.h"

#define DOCWIRE_MAKE_ERROR_SINGLE(v) errors::impl { errors::convert_to_context(BOOST_PP_STRINGIZE(v), v) }
#define DOCWIRE_MAKE_ERROR_FIRST(...) DOCWIRE_MAKE_ERROR_SINGLE(BOOST_PP_SEQ_HEAD(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define DOCWIRE_MAKE_ERROR_ITER(r, data, i, elem) \
	BOOST_PP_COMMA_IF(i) \
	DOCWIRE_MAKE_ERROR_SINGLE(elem)

#define DOCWIRE_MAKE_ERROR(...) \
	BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1), \
		(DOCWIRE_MAKE_ERROR_FIRST(__VA_ARGS__)), \
		errors::make_nested( \
			BOOST_PP_SEQ_FOR_EACH_I(DOCWIRE_MAKE_ERROR_ITER, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
		) \
	)

#define DOCWIRE_MAKE_ERROR_PTR(...) \
	std::make_exception_ptr(make_error(__VA_ARGS__))

#ifdef DOCWIRE_ENABLE_SHORT_MACRO_NAMES
#define make_error DOCWIRE_MAKE_ERROR
#define make_error_ptr DOCWIRE_MAKE_ERROR_PTR
#endif

#endif
