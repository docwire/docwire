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

#include "tracing.h"

#include <stdio.h>
#include <stdlib.h>

static int level = 0;
static FILE* file = NULL;

void docwire_init_tracing(const char* filename) __attribute__((no_instrument_function));
static void close_tracing(void)  __attribute__((no_instrument_function, destructor));
static void trace_func(bool call, void* func, void* caller) __attribute__((no_instrument_function));

void docwire_init_tracing(const char* filename)
{
	file = fopen(filename, "w");
	if (file == NULL)
	{
		fprintf(stderr, "Error opening %s trace file.\n", filename);
		exit(1);
	}
}

static void close_tracing(void)
{
	if (file != NULL)
		fclose(file);
	file = NULL;
}

static void trace_func(bool call, void* func, void* caller)
{
	if (!call)
		level--;
	if (file != NULL)
	{
		// warning TODO: Make this thread safe and add thread id to output.
		fprintf(file, "%i %c %p %p\n", level, call ? 'C' : 'R', func, caller);
		fflush(file);
	}
	if (call)
		level++;
}

extern "C"
{
	void __cyg_profile_func_enter(void* func,  void* caller) __attribute__((no_instrument_function));
	void __cyg_profile_func_exit (void* func, void* caller) __attribute__((no_instrument_function));

	void __cyg_profile_func_enter(void* func, void* caller)
	{
		trace_func(true, func, caller);
	}

	void __cyg_profile_func_exit (void* func, void* caller)
	{
		trace_func(false, func, caller);
	}
}
