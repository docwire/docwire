/***************************************************************************************************************************************************/
/*  DocToText - A multifaceted, data extraction software development toolkit that converts all sorts of files to plain text and html.              */
/*  Written in C++, this data extraction tool has a parser able to convert PST & OST files along with a brand new API for better file processing.  */
/*  To enhance its utility, DocToText, as a data extraction tool, can be integrated with other data mining and data analytics applications.        */
/*  It comes equipped with a high grade, scriptable and trainable OCR that has LSTM neural networks based character recognition.                   */
/*                                                                                                                                                 */
/*  This document parser is able to extract metadata along with annotations and supports a list of formats that include:                           */
/*  DOC, XLS, XLSB, PPT, RTF, ODF (ODT, ODS, ODP), OOXML (DOCX, XLSX, PPTX), iWork (PAGES, NUMBERS, KEYNOTE), ODFXML (FODP, FODS, FODT),           */
/*  PDF, EML, HTML, Outlook (PST, OST), Image (JPG, JPEG, JFIF, BMP, PNM, PNG, TIFF, WEBP) and DICOM (DCM)                                         */
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

#ifndef DOCTOTEXT_THREAD_SAFE_OLE_STORAGE_H
#define DOCTOTEXT_THREAD_SAFE_OLE_STORAGE_H

#include <string>
#include <vector>
#include "wv2/olestorage.h"
#include "defines.h"

class ThreadSafeOLEStreamReader;
using namespace wvWare;

class DllExport ThreadSafeOLEStorage : public AbstractOLEStorage
{
	private:
		struct Implementation;
		Implementation* impl;
	public:
		explicit ThreadSafeOLEStorage(const std::string& file_name);
		ThreadSafeOLEStorage(const char* buffer, size_t len);
		~ThreadSafeOLEStorage() override;
		bool isValid() const override;
		bool open(Mode mode) override;
		void close() override;
		std::string name() const override;
		std::string getLastError();
		bool getStreamsAndStoragesList(std::vector<std::string>& components);
		bool enterDirectory(const std::string& directory_path);
		bool leaveDirectory();
		bool readDirectFromBuffer(unsigned char* buffer, int size, int offset) override;
		AbstractOLEStreamReader* createStreamReader(const std::string& stream_path) override;
	private:
		void streamDestroyed(OLEStream* stream) override;
};

#endif // DOCTOTEXT_THREAD_SAFE_OLE_STORAGE_H
