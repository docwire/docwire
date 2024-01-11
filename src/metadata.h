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

#ifndef DOCWIRE_METADATA_H
#define DOCWIRE_METADATA_H

#include <string>
#include <map>
#include <any>
#include "variant.h"
#include "defines.h"

namespace docwire
{
	class DllExport Metadata
	{
		private:
			struct Implementation;
			Implementation* impl;

		public:
			enum DataType { NONE, EXTRACTED };
			Metadata();
			Metadata(const Metadata& r);
			~Metadata();
			Metadata& operator=(const Metadata& r);

			DataType authorType();
			void setAuthorType(DataType type);

			const char* author();
			void setAuthor(const std::string& author);

			DataType creationDateType();
			void setCreationDateType(DataType type);

			const tm& creationDate();
			void setCreationDate(const tm& creation_date);

			DataType lastModifiedByType();
			void setLastModifiedByType(DataType type);

			const char* lastModifiedBy();
			void setLastModifiedBy(const std::string& last_modified_by);

			DataType lastModificationDateType();
			void setLastModificationDateType(DataType type);

			const tm& lastModificationDate();
			void setLastModificationDate(const tm& last_modification_date);

			DataType pageCountType();
			void setPageCountType(DataType type);

			int pageCount();
			void setPageCount(int page_count);

			DataType wordCountType();
			void setWordCountType(DataType type);

			int wordCount();
			void setWordCount(int word_count);

			void addField(const std::string& field_name, const Variant& field_value);
			bool hasField(const std::string& field_name) const;
			const Variant& getField(const std::string& field_name) const;

			const std::map<std::string, Variant>& getFields() const;
			const std::map<std::string, std::any> getFieldsAsAny() const;
	};
} // namespace docwire

#endif
