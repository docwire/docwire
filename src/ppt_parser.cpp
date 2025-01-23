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

#include "ppt_parser.h"

#include "error_tags.h"
#include "log.h"
#include <math.h>
#include "misc.h"
#include "oshared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "throw_if.h"
#include <wv2/ustring.h>
#include "wv2/textconverter.h"
#include <vector>
#include <stack>
#include "thread_safe_ole_stream_reader.h"
#include "thread_safe_ole_storage.h"

namespace docwire
{

using namespace wvWare;

enum RecordType
{
	RT_CSTRING = 0xFBA,
	RT_DOCUMENT = 0x03E8,
	RT_DRAWING = 0x040C,
	RT_END_DOCUMENT_ATOM = 0x03EA,
	RT_LIST = 0x07D0,
	RT_MAIN_MASTER = 0x03F8,
	RT_SLIDE_BASE = 0x03EC, // Not in MS specification
	RT_SLIDE_LIST_WITH_TEXT = 0x0FF0,
	RT_SLIDE = 0x03EE,
	RT_TEXT_BYTES_ATOM = 0x0FA8,
	RT_TEXT_CHARS_ATOM = 0x0FA0,
	OFFICE_ART_CLIENT_TEXTBOX = 0xF00D,
	OFFICE_ART_DG_CONTAINER = 0xF002,
	OFFICE_ART_SPGR_CONTAINER = 0xF003,
	OFFICE_ART_SP_CONTAINER = 0xF004
};

namespace
{

	U16 getU16LittleEndian(std::vector<unsigned char>::const_iterator buffer)
	{
		return (unsigned short int)(*buffer) | ((unsigned short int)(*(buffer + 1)) << 8);
	}

	U32 getU32LittleEndian(std::vector<unsigned char>::const_iterator buffer)
	{
		return (unsigned long)(*buffer) | ((unsigned long)(*(buffer +1 )) << 8L) | ((unsigned long)(*(buffer + 2)) << 16L) | ((unsigned long)(*(buffer + 3)) << 24L);
	}

	void parseRecord(int rec_type, unsigned long rec_len, ThreadSafeOLEStreamReader& reader, std::string& text, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
	{
		switch(rec_type)
		{
			case RT_CSTRING:
			case RT_TEXT_CHARS_ATOM: 
			{
				docwire_log(debug) << "RT_TextCharsAtom or RT_CString";
				std::vector<unsigned char> buf(2);
				unsigned long text_len = rec_len / 2;
				if (text_len * 2 > reader.size() - reader.tell())
				{
					text_len = (reader.size() - reader.tell()) / 2;
					non_fatal_error_handler(make_error_ptr("Read past EOF"));
				}
				for (int i = 0; i < text_len; i++)
				{
					reader.read((uint8_t*)&*buf.begin(), 2);
					U32 u = getU16LittleEndian(buf.begin());
					if (u == 0x0D || u == 0x0B)
						text += '\n';
					else
					{
						if (utf16_unichar_has_4_bytes(u) && ++i < text_len)
						{
							reader.read((uint8_t*)&*buf.begin(), 2);
							u = (u << 16) | getU16LittleEndian(buf.begin());
						}
						text += unichar_to_utf8(u);
					}
				}
				text += '\n';
				break;
			}
			case RT_DOCUMENT:
				docwire_log(debug) << "RT_Document";
				break;
			case RT_DRAWING:
				docwire_log(debug) << "RT_Drawing";
				break;
			case RT_END_DOCUMENT_ATOM:
			{
				docwire_log(debug) << "RT_DocumentEnd";
				unsigned long len = rec_len;
				if (reader.tell() + len > reader.size())
				{
					non_fatal_error_handler(make_error_ptr("Read past EOF"));
					len = reader.size() - reader.tell();
				}
				reader.seek(len, SEEK_CUR);
				break;
			}
			case RT_LIST:
				docwire_log(debug) << "RT_List";
				break;
			case RT_MAIN_MASTER:
			{
				docwire_log(debug) << "RT_MainMaster";
				// warning TODO: Make extracting text from main master slide configurable
				unsigned long len = rec_len;
				if (reader.tell() + len > reader.size())
				{
					non_fatal_error_handler(make_error_ptr("Read past EOF"));
					len = reader.size() - reader.tell();
				}
				reader.seek(len, SEEK_CUR);
				break;
			}
			case RT_SLIDE:
				docwire_log(debug) << "RT_Slide";
				break;
			case RT_SLIDE_BASE:
				break;
			case RT_SLIDE_LIST_WITH_TEXT:
				docwire_log(debug) << "RT_SlideListWithText";
				break;
			case RT_TEXT_BYTES_ATOM:
			{
				docwire_log(debug) << "RT_TextBytesAtom";
				std::vector<unsigned char> buf(2);
				unsigned long text_len = rec_len;
				buf[0] = buf[1] = 0;
				if (text_len > reader.size() - reader.tell())
				{
					text_len = reader.size() - reader.tell();
					non_fatal_error_handler(make_error_ptr("Read past EOF"));
				}
				for (int i = 0; i < text_len; i++)
				{
					reader.read((uint8_t*)&*buf.begin(), 1);
					U32 u = getU16LittleEndian(buf.begin());
					if (u == 0x0B || u == 0x0D)
						text += '\n';
					else
						text += unichar_to_utf8(u);
				}
				text += '\n';
				break;
			}
			case OFFICE_ART_CLIENT_TEXTBOX:
				docwire_log(debug) << "OfficeArtClientTextbox";
				break;
			case OFFICE_ART_DG_CONTAINER:
				docwire_log(debug) << "OfficeArtDgContainer";
				break;
			case OFFICE_ART_SPGR_CONTAINER:
				docwire_log(debug) << "OfficeArtSpgrContainer";
				break;
			case OFFICE_ART_SP_CONTAINER:
				docwire_log(debug) << "OfficeArtSpContainer";
				break;
			default:
				unsigned long len = rec_len;
				if (reader.tell() + len > reader.size())
				{
					non_fatal_error_handler(make_error_ptr("Read past EOF"));
					len = reader.size() - reader.tell();
				}
				reader.seek(len, SEEK_CUR);
		}
		throw_if (!reader.isValid(), reader.getLastError());
	}

	bool oleEof(ThreadSafeOLEStreamReader& reader)
	{
		return reader.tell() == reader.size();
	}

	void parseOldPPT(ThreadSafeOLEStorage& storage, ThreadSafeOLEStreamReader& reader, std::string& text, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
	{
		std::vector<unsigned char> content(reader.size());
		throw_if (!reader.read(&*content.begin(), reader.size()));	//this stream should only contain text
		text = std::string(content.begin(), content.end());
		try
		{
			std::string codepage = get_codepage_from_document_summary_info(storage);
			TextConverter tc(codepage);
			text = ustring_to_string(tc.convert(text));
		}
		catch (const std::exception& e)
		{
			non_fatal_error_handler(errors::make_nested_ptr(e, make_error("Error converting text")));
		}
		//new lines problem?
		for (size_t i = 0; i < text.length(); ++i)
			if (text[i] == 11)
				text[i] = 13;
	}

	void parsePPT(ThreadSafeOLEStreamReader& reader, std::string& text, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
	{
		std::vector<unsigned char> rec(8);
		bool read_status = true;
		std::stack<long> container_ends;
		while (read_status)
		{
			int pos = reader.tell();
			read_status = reader.read(&*rec.begin(), 8);
			if (rec.size() < 8)
				break;
			if (oleEof(reader))
			{
				parseRecord(RT_END_DOCUMENT_ATOM, 0, reader, text, non_fatal_error_handler);
				return;
			}
			int rec_type = getU16LittleEndian(rec.begin() + 2);
			U32 rec_len = getU32LittleEndian(rec.begin() + 4);
			if (log_verbosity_includes(debug))
			{
				while (!container_ends.empty() && pos+rec_len-1 > container_ends.top())
					container_ends.pop();
				std::string indend;
				for (int i = 0; i < container_ends.size(); i++)
					indend += "\t";
				docwire_log(debug) << indend << "record" << hex() << rec_type << "begin" << pos << "end" << pos + rec_len - 1;
				container_ends.push(pos + rec_len - 1);
			}
			try
			{
				parseRecord(rec_type, rec_len, reader, text, non_fatal_error_handler);
			}
			catch (const std::exception& e)
			{
				std::throw_with_nested(make_error(rec_type, rec_len));
			}
		}
	}

	void assertFileIsNotEncrypted(ThreadSafeOLEStorage& storage)
	{
		std::vector<std::string> dirs;
		if (storage.getStreamsAndStoragesList(dirs))
		{
			for (size_t i = 0; i < dirs.size(); ++i)
			{
				if (dirs[i] == "EncryptedSummary")
				{
					//this is very easy way to detect if file is encrypted: just to check if EncryptedSummary stream exist.
					//This stream is obligatory in encrypted files and prohibited in non-encrypted files.
					throw make_error(errors::file_encrypted{});
				}
			}
		}
	}
};

PPTParser::PPTParser()
{
}

void PPTParser::parse(const data_source& data)
{	
	docwire_log(debug) << "Using PPT parser.";
	try
	{
		std::unique_ptr<ThreadSafeOLEStorage> storage = std::make_unique<ThreadSafeOLEStorage>(data.span());
		throw_if (!storage->isValid(), "Error opening stream as OLE container");
		assertFileIsNotEncrypted(*storage);
		sendTag(tag::Document
			{
				.metadata = [this, &storage]() { return metaData(storage); }
			});
		std::string text;
		std::vector<std::string> dirs;
		if (storage->getStreamsAndStoragesList(dirs))
		{
			for (size_t i = 0; i < dirs.size(); ++i)
			{
				// warning TODO: Older PPT files (like Microsoft PowerPoint 4.0) have stream named "Text_Content". \
				I have only one file of this type and I cant find any documentation about it. I am not sure \
				if parsing those files is correct now... but for this one file that I have it works. It should be tested \
				more, one file is not enough. Stream "Text_Content" does not exist in newer versions of PowerPoint. \
				In older versions of PowerPoint (the file I have is an example) reading stream "PowerPoint Document" does not work.
				// warning TODO: Check if there is a better way to get PowerPoint version.
				if (dirs[i] == "Text_Content")
				{
					std::unique_ptr<ThreadSafeOLEStreamReader> reader { (ThreadSafeOLEStreamReader*)storage->createStreamReader("Text_Content") };
					throw_if (reader == NULL, storage->getLastError(), std::make_pair("stream_path", "Text_Content"));
					parseOldPPT(*storage, *reader, text, [this](std::exception_ptr e) { sendTag(e); });
					sendTag(tag::Text{.text = text});
					return;
				}
			}
		}
		std::unique_ptr<ThreadSafeOLEStreamReader> reader { (ThreadSafeOLEStreamReader*)storage->createStreamReader("PowerPoint Document") };
		throw_if (reader == NULL, storage->getLastError(), std::make_pair("stream_path", "PowerPoint Document"));
		parsePPT(*reader, text, [&](std::exception_ptr e) { sendTag(e); });
		sendTag(tag::Text{.text = text});
		sendTag(tag::CloseDocument{});
		return;
	}
	catch (const std::exception& e)
	{
		std::throw_with_nested(make_error("Error parsing PPT document"));
	}
}

attributes::Metadata PPTParser::metaData(const std::unique_ptr<ThreadSafeOLEStorage>& storage) const
{
	attributes::Metadata meta;
		parse_oshared_summary_info(*storage, meta, [this](std::exception_ptr e) { sendTag(e); });
		// If page count not found use slide count as page count
		if (!meta.page_count)
		{
			int slide_count = 150;
			try
			{
				parse_oshared_document_summary_info(*storage, slide_count);
				if(slide_count!=-1){
					meta.page_count = slide_count;
				}
			}
			catch (const std::exception& e)
			{
				sendTag(std::current_exception());
			}
		}
		return meta;
}

} // namespace docwire
