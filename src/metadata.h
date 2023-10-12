/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP), Archives (ZIP, TAR, RAR, GZ, BZ2, XZ)                  */
/*  and DICOM (DCM)                                                                                                                                */
/*                                                                                                                                                 */
/*  Copyright (c) SILVERCODERS Ltd                                                                                                                 */
/*  http://silvercoders.com                                                                                                                        */
/*                                                                                                                                                 */
/*  Project homepage:                                                                                                                              */
/*  http://silvercoders.com/en/products/doctotext                                                                                                  */
/*  https://www.docwire.io/                                                                                                                        */
/*                                                                                                                                                 */
/*  The GNU General Public License version 2 as published by the Free Software Foundation and found in the file COPYING.GPL permits                */
/*  the distribution and/or modification of this application.                                                                                      */
/*                                                                                                                                                 */
/*  Please keep in mind that any attempt to circumvent the terms of the GNU General Public License by employing wrappers, pipelines,               */
/*  client/server protocols, etc. is illegal. You must purchase a commercial license if your program, which is distributed under a license         */
/*  other than the GNU General Public License version 2, directly or indirectly calls any portion of this code.                                    */
/*  Simply stop using the product if you disagree with this viewpoint.                                                                             */
/*                                                                                                                                                 */
/*  According to the terms of the license provided by SILVERCODERS and included in the file COPYING.COM, licensees in possession of                */
/*  a current commercial license for this product may use this file.                                                                               */
/*                                                                                                                                                 */
/*  This program is provided WITHOUT ANY WARRANTY, not even the implicit warranty of merchantability or fitness for a particular purpose.          */
/*  It is supplied in the hope that it will be useful.                                                                                             */
/***************************************************************************************************************************************************/

#ifndef DOCTOTEXT_METADATA_H
#define DOCTOTEXT_METADATA_H

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
