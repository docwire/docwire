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

#include "decompress_archives.h"

#include "libarchive_cpp_wrapper.hpp"
#include "exception.h"
#include <filesystem>
#include <fstream>
#include "parser.h"

namespace doctotext
{

DecompressArchives::DecompressArchives()
{
}

DecompressArchives::DecompressArchives(const DecompressArchives &other)
{
}

DecompressArchives::~DecompressArchives()
{
}

void
DecompressArchives::process(doctotext::Info &info) const
{
	//std::cerr << "Decompress archives" << std::endl;
	if (info.tag_name != StandardTag::TAG_FILE)
	{
		emit(info);
		return;
	}
	//std::cerr << "TAG_FILE" << std::endl;
	std::optional<std::string> path = info.getAttributeValue<std::string>("path");
	std::optional<std::istream*> stream = info.getAttributeValue<std::istream*>("stream");
	std::optional<std::string> name = info.getAttributeValue<std::string>("name");
	/*if (name)
		std::cerr << "Name: " << *name << std::endl;*/
	if(!path && !stream)
		throw Exception("No path or stream in TAG_FILE");
	auto is_supported = [](const std::string& fn)
	{
		std::set<std::string> supported_extensions { ".zip", ".tar", ".rar", ".gz", ".bz2", ".xz" };
		return supported_extensions.count(std::filesystem::path(fn).extension().string()) > 0;
	};
	if ((path && !is_supported(*path)) || (name && !is_supported(*name)))
	{
		//std::cerr << "Filename extension shows it is not an supported archive, skipping." << std::endl;
		emit(info);
		return;
	}
	std::istream* in_stream = path ? new std::ifstream ((*path).c_str(), std::ios::binary ) : *stream;
	try
	{
		namespace ar = ns_archive::ns_reader;
		ns_archive::reader reader = ns_archive::reader::make_reader<ar::format::_ALL, ar::filter::_ALL>(*in_stream, 10240);
		//std::cerr << "Decompressing archive" << std::endl;
		for(auto entry : reader)
		{
			if (entry->get_header_value_mode() & AE_IFDIR)
				continue;
			//std::cerr << "Processing compressed file " << entry->get_header_value_pathname() << std::endl;
			Info info(StandardTag::TAG_FILE, "", {{"stream", &entry->get_stream()}, {"name", entry->get_header_value_pathname()}});
			process(info);
			//std::cerr << "End of processing compressed file " << entry->get_header_value_pathname() << std::endl;
		}
	}
	catch(ns_archive::archive_exception& e)
	{
		//std::cerr << e.what() << std::endl;
		in_stream->clear();
		in_stream->seekg(std::ios::beg);
		emit(info);
	}
	if (path)
		delete in_stream;
}

DecompressArchives* DecompressArchives::clone() const
{
	return new DecompressArchives(*this);
}

} // namespace doctotext
