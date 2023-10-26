/***************************************************************************************************************************************************/
/*  DocWire SDK - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.            */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocWire, as a data extraction tool, can be integrated with other data mining and data analytics applications.          */
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
/*  https://github.com/docwire/docwire                                                                                                             */
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

#ifndef DOCWIRE_HTTP_POST_H
#define DOCWIRE_HTTP_POST_H

#include "chain_element.h"
#include "exception.h"

namespace docwire
{
namespace http
{

class DllExport Post : public ChainElement
{
public:
	Post(const std::string& url, const std::string& oauth2_bearer_token = "");
	Post(const Post& other);
	virtual ~Post();

	/**
	* @brief Executes transform operation for given node data.
	* @see docwire::Info
	* @param info
	*/
	void process(Info &info) const;

	bool is_leaf() const override
	{
		return false;
	}

	/**
	* @brief Creates clone of the Post
	* @return new Post
	*/
	Post* clone() const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl;
};

class LogicError : public docwire::LogicError
{
public:
	LogicError(const std::string& message) : docwire::LogicError(message) {}
	LogicError(const std::string& message, const std::exception& nested) : docwire::LogicError(message, nested) {}
};

class RuntimeError : public docwire::RuntimeError
{
public:
	RuntimeError(const std::string& message) : docwire::RuntimeError(message) {}
	RuntimeError(const std::string& message, const std::exception& nested) : docwire::RuntimeError(message, nested) {}
};

} // namespace http
} // namespace docwire

#endif //DOCWIRE_HTTP_POST_H
