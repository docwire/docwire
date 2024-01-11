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

#ifndef DOCWIRE_VARIANT_H
#define DOCWIRE_VARIANT_H

#include <string>
#include <time.h>
#include "defines.h"

namespace docwire
{
	/**
		Variant is an object which can represent unsigned number, date or string.
		Uninitialized object represents 'null'.
	**/
	class DllExport Variant
	{
		private:
			struct Implementation;
			Implementation* impl;

		public:

			///Creates uninitialized object (represents null)
			Variant();

			///Creates object which represents string
			Variant(const std::string& value);

			///Creates object which represents date
			Variant(const tm& value);

			///Creates object which represents unsigned number
			Variant(size_t value);

			///Creates a copy of another Variant
			Variant(const Variant& variant);
			~Variant();
			Variant& operator = (const Variant& variant);

			///Sets unsigned number value. From this moment, Variant represents unsigned number.
			void setValue(size_t number);

			///Sets date value. From this moment, Variant represents date.
			void setValue(const tm& date_time);

			///Sets string value. From this moment, Variant represents string.
			void setValue(const std::string& str);

			///If Variant has not been initialized with number/date/string, this method returns value true
			bool isEmpty() const;

			///Checks if Variant represents a string
			bool isString() const;

			///Checks if Variant represents a number
			bool isNumber() const;

			///Checks if Variant represents a date
			bool isDateTime() const;

			/**
				If Variant is a date, this method returns const reference to tm object. But if it is not
				a date, struct tm can be filled with undefined numbers (usually zeros)
			**/
			const tm& getDateTime() const;

			/**
				Returns string value. If Variant is a string, then string is returned (simple). But if variant represents
				number or a date, proper value is converted to the string. If variant has no value (represents null),
				empty string is returned.
			**/
			const char* getString() const;

			/**
				If Variant is a number, then number which is inside this object is returned. In other cases,
				returned number may be undefined (usually 0).
			**/
			size_t getNumber() const;
	};
} // namespace docwire

#endif
