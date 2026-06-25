/*********************************************************************************************************************************************/
/*  DocWire SDK: Award-winning modern data processing in C++20. SourceForge Community Choice & Microsoft support. AI-driven processing.      */
/*  Supports nearly 100 data formats, including email boxes and OCR. Boost efficiency in text extraction, web data extraction, data mining,  */
/*  document analysis. Offline processing possible for security and confidentiality                                                          */
/*                                                                                                                                           */
/*  Copyright (c) SILVERCODERS Ltd, http://silvercoders.com                                                                                  */
/*  Project homepage: https://github.com/docwire/docwire                                                                                     */
/*                                                                                                                                           */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DocWire-Commercial                                                                  */
/*********************************************************************************************************************************************/

#include "doc_parser.h"

#include "document_elements.h"
#include "error_tags.h"
#include "log_cerr_redirection.h"
#include "log_entry.h"
#include "log_scope.h"
#include "misc.h"
#include "nested_exception.h"
#include "throw_if.h"
#include "wv2/src/parser.h"
#include "wv2/src/parser9x.h"
#include "wv2/src/fields.h"
#include "wv2/src/handlers.h"
#include <mutex>
#include "oshared.h"
#include "wv2/src/paragraphproperties.h"
#include "wv2/src/parserfactory.h"
#include <stdio.h>
#include "wv2/src/ustring.h"
#include <vector>
#ifdef WIN32
	#include <windows.h>
#endif
#include "wv2/src/word_helper.h"
#include "wv2/src/word97_generated.h"
#include "xls_parser.h"
#include "serialization_data_source.h" // IWYU pragma: keep
#include "serialization_enum.h" // IWYU pragma: keep
#include "thread_safe_ole_stream_reader.h"
#include "thread_safe_ole_storage.h"

namespace docwire
{

using namespace wvWare;

namespace
{
	std::mutex parser_factory_mutex_1;
	std::mutex parser_factory_mutex_2;

const std::vector<mime_type> supported_mime_types =
{
    mime_type{"application/msword"}
};

} // anonymous namespace

template<>
struct pimpl_impl<doc_parser> : pimpl_impl_base
{
    void parse(const data_source& data, const message_callbacks& emit_message);
};

enum class table_state
{
	in_table,
	in_row,
	in_cell
};

struct current_header_footer
{
	bool in_header;
	bool in_footer;
	current_header_footer() : in_header(false), in_footer(false) {};
};

// constants are from MS Word Binary File Format Specification
enum field_type
{
	FLT_NONE = 0,
	FLT_FILLIN = 0x27,
	FLT_EMBED = 0x3A,
	FLT_HYPERLINK = 0x58
};

enum field_part
{
	FIELD_PART_NONE,
	FIELD_PART_PARAMS,
	FIELD_PART_VALUE
};

struct current_state
{
	std::stack<docwire::table_state> table_state;
	current_header_footer header_footer;
	std::list<std::string> obj_texts;
	std::list<std::string>::iterator obj_texts_iter;
	docwire::field_type field_type;
	docwire::field_part field_part;
	UString field_params;
	UString field_value;
	current_state() : field_type(FLT_NONE), field_part(FIELD_PART_NONE) {};
};

struct comment
{
	int fc;
	std::string text;
	std::string author;
};

static void cp_to_stream_offset(const wvWare::Parser* parser, U32 cp, U32& stream_offset, bool* unicode_detected = NULL)
{
	log_scope(cp);
	const Parser9x* parser9 = dynamic_cast<const Parser9x*>(parser);
	throw_if (parser9 == NULL, "This is not a 9x parser.", errors::program_logic{});
	U32 piece = 0;
	U32 offset = cp;
	throw_if (parser9->pieceTable() == nullptr, "No pieces table found.", errors::uninterpretable_data{});
	// Copied from wv2 code.
	PLCFIterator<Word97::PCD> it( *parser9->pieceTable() );
	for ( ; it.current(); ++it, ++piece ) {
		if ( it.currentLim() > cp && it.currentStart() <= cp )
			break;
		offset -= it.currentRun();
	}
	log_entry(piece, offset);
	PLCFIterator<Word97::PCD> it2(parser9->pieceTable()->at( piece ) );
	throw_if (!it2.current(), "Specified piece not found.", errors::uninterpretable_data{});
	U32 piece_fc = it2.current()->fc;
	log_entry(piece_fc);
	bool unicode;
	U32 real_fc = piece_fc;
	parser9->calculateRealFC(real_fc, unicode);
	log_entry(real_fc, unicode);
	U32 final_stream_offset = real_fc + (unicode ? offset * 2 : offset);
	log_entry(final_stream_offset);
	stream_offset = final_stream_offset;
	if (unicode_detected != NULL)
		*unicode_detected = unicode;
}

static void parse_comments(const wvWare::Parser* parser, std::vector<comment>& comments, const std::function<void(std::exception_ptr)>& non_fatal_error_handler)
{
	log_scope();
	try
	{
		if (parser->fib().lcbPlcfandTxt == 0)
		{
			log_entry();
			return;
		}

		U32 atn_part_cp = parser->fib().ccpText + parser->fib().ccpFtn + parser->fib().ccpHdd + parser->fib().ccpMcr;
		log_entry(atn_part_cp);
		std::unique_ptr<OLEStreamReader> reader { parser->storage()->createStreamReader("WordDocument") };
		throw_if (!reader, "Error opening WordDocument stream.", errors::uninterpretable_data{});
		const Parser9x* parser9 = dynamic_cast<const Parser9x*>(parser);
		std::unique_ptr<OLEStreamReader> table_reader { parser->storage()->createStreamReader(parser9->tableStream()) };
		throw_if (!table_reader, "Error opening table stream.", errors::uninterpretable_data{});

		U32 annotation_txts_offset = parser->fib().fcPlcfandTxt;
		log_entry(annotation_txts_offset);
		table_reader->seek(annotation_txts_offset);
		U32 annotation_begin_cp = table_reader->readU32();
		std::vector<std::string> annotations;
		for (;;)
		{
			U32 annotation_end_cp = table_reader->readU32();
			log_scope(annotation_begin_cp, annotation_end_cp);
			U32 stream_begin_offset;
			bool unicode;
			try
			{
				cp_to_stream_offset(parser, atn_part_cp + annotation_begin_cp, stream_begin_offset, &unicode);
			}
			catch (const std::exception&)
			{
				std::throw_with_nested(make_error("Converting annotation start position to stream offset failed.", errors::uninterpretable_data{}));
			}
			U32 stream_end_offset;
			try
			{
				cp_to_stream_offset(parser, atn_part_cp + annotation_end_cp, stream_end_offset);
			}
			catch (const std::exception&)
			{
				std::throw_with_nested(make_error("Converting annotation end position to stream offset failed.", errors::uninterpretable_data{}));
			}
			log_entry(stream_begin_offset, stream_end_offset);
			reader->seek(stream_begin_offset);
			U8 annotation_mark = reader->readU8();
			throw_if (annotation_mark != 0x05, "Incorrect annotation mark.", errors::uninterpretable_data{});
			std::string annotation;
			while (reader->tell() < stream_end_offset - 1)
			{
				log_scope(reader->tell());
				// warning TODO: Unicode support in comments
				if (unicode)
					reader->seek(1, SEEK_CUR); // skip unicode byte
				S8 ch = reader->readS8();
				if (ch >= 32 || (ch >= 8 && ch <= 13))
				{
					if (ch == '\r')
						annotation += "\n";
					else
						annotation += ch;
				}
			}
			log_entry(annotation);
			annotations.push_back(annotation);
			if (annotation_end_cp >= parser->fib().ccpAtn - 1)
				break;
			annotation_begin_cp = annotation_end_cp;
		}

		U32 annotation_owners_offset = parser->fib().fcGrpXstAtnOwners;
		U32 annotation_owners_len = parser->fib().lcbGrpXstAtnOwners;
		log_entry(annotation_owners_offset, annotation_owners_len);
		table_reader->seek(annotation_owners_offset);
		std::vector<std::string> owners;
		for (int total_len = 0; total_len < annotation_owners_len;)
		{
			U16 len = table_reader->readU16();
			total_len += 2;
			std::string owner;
			if (len * 2 + total_len > annotation_owners_len)
			{
				non_fatal_error_handler(make_error_ptr("Something is wrong with XST table. Authors of comments could not be parsed."));
				owners.clear();
				break;
			}
			for (int i = 0; i < len; i++)
			{
				S16 ch = table_reader->readS16();
				owner += (char)ch;
				total_len += 2;
			}
			log_entry(owner);
			owners.push_back(owner);
		}

		U32 annotation_refs_offset = parser->fib().fcPlcfandRef;
		log_entry(annotation_refs_offset);
		table_reader->seek(annotation_refs_offset);
		for (int i = 0;; i++)
		{
			log_scope(i);
			U32 annotation_ref_cp = table_reader->readU32();
			if (annotation_ref_cp >= parser->fib().ccpText)
				break;
			log_entry(i, annotation_ref_cp);
			U32 stream_offset;
			try
			{
				cp_to_stream_offset(parser, annotation_ref_cp, stream_offset);
			}
			catch (const std::exception&)
			{
				std::throw_with_nested(make_error("Converting annotation reference position to stream offset failed.", errors::uninterpretable_data{}));
			}
			if (i < annotations.size())
			{
				log_scope(i, stream_offset);
				comment c;
				c.fc = stream_offset;
				c.text = annotations[i];
				comments.push_back(c);
			}
		}
		table_reader->seek(annotation_refs_offset + 4 * (comments.size() + 1));
		for (int i = 0; i < comments.size(); i++)
		{
			Word97::ATRD atrd(table_reader.get());
			if (atrd.ibst < owners.size())
			{
				log_scope(owners[atrd.ibst], atrd.ibst);
				comments[i].author = owners[atrd.ibst];
			}
		}
	}
	catch (std::bad_alloc& ba)
	{
		throw;
	}
}

class text_handler : public wvWare::TextHandler
{
	private:
		const message_callbacks& m_emit_message;
		const wvWare::Parser* m_parser;
		current_state* m_curr_state;
		bool m_comments_parsed;
		std::vector<comment> m_comments;
		U32 m_prev_par_fc;

	public:
		text_handler(const message_callbacks& emit_message, const wvWare::Parser* parser, current_state* curr_state)
			: m_emit_message(emit_message), m_parser(parser), m_curr_state(curr_state), m_comments_parsed(false), m_prev_par_fc(0)
		{
			m_curr_state = curr_state;
		}

		void sectionStart(SharedPtr<const Word97::SEP> sep)
		{
			log_scope();
		}

		void sectionEnd()
		{
		}

		void pageBreak()
		{
			log_scope();
		}

		void paragraphStart(SharedPtr<const ParagraphProperties>
			paragraphProperties)
		{
			log_scope();
			if (!m_curr_state->table_state.empty())
			{
				if (m_curr_state->table_state.top() == table_state::in_table)
				{
					m_emit_message(document::close_table{});
					m_curr_state->table_state.pop();
				}
				else if (m_curr_state->table_state.top() == table_state::in_row)
				{
					m_emit_message(document::table_cell{});
					m_curr_state->table_state.push(table_state::in_cell);
				}
			}
			m_emit_message(document::paragraph{});
			if (((Parser9x*)m_parser)->currentParagraph()->size() > 0)
			{
				if (m_comments_parsed)
				{
					for (int i = 0; i < m_comments.size(); i++)
					if (m_comments[i].fc >= m_prev_par_fc && m_comments[i].fc < ((Parser9x*)m_parser)->currentParagraph()->back().m_startFC)
					{
						std::string comment_text = m_comments[i].text;
						std::replace(comment_text.begin(), comment_text.end(), '\x0b', '\n');
						m_emit_message(document::comment{.author = m_comments[i].author, .comment = comment_text});
					}
				}
				else
				{
					try
					{
						parse_comments(m_parser, m_comments, [&](std::exception_ptr e) { m_emit_message(std::move(e)); }); // ?
					}
					catch (std::exception&)
					{
						m_emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Parsing comments failed.")));
						m_comments.clear();
					}
					m_comments_parsed = true;
				}
			}
		}

		void paragraphEnd()
		{
			log_scope();
			m_emit_message(document::close_paragraph{});
			if (!((Parser9x*)m_parser)->currentParagraph()->empty())
			{
				m_prev_par_fc = ((Parser9x*)m_parser)->currentParagraph()->back().m_startFC;
			}
		}

		void runOfText (const UString &text, SharedPtr< const Word97::CHP > chp)
		{
			log_scope();
			if (m_curr_state->field_part == FIELD_PART_PARAMS)
				m_curr_state->field_params += text;
			else if (m_curr_state->field_part == FIELD_PART_VALUE)
				m_curr_state->field_value += text;
			else
			{
				std::string t = ustring_to_string(text);
				std::replace(t.begin(), t.end(), '\x0b', '\n');
				m_emit_message(document::text{.text = t});
			}
		}

		void specialCharacter(SpecialCharacter character,
			SharedPtr<const Word97::CHP> chp)
		{
			log_scope(character);
		}

		void footnoteFound (FootnoteData::Type type, UChar character,
			SharedPtr<const Word97::CHP> chp,
			const FootnoteFunctor &parseFootnote)
		{
		}

		void footnoteAutoNumber(SharedPtr<const Word97::CHP> chp)
		{
			log_scope();
		}

		void fieldStart(const FLD *fld,
			SharedPtr<const Word97::CHP> chp)
		{
			log_scope();
			m_curr_state->field_type = (field_type)fld->flt;
			m_curr_state->field_part = FIELD_PART_PARAMS;
			switch (fld->flt)
			{
				case FLT_EMBED:
				{
					log_scope();
					if (m_curr_state->obj_texts_iter == m_curr_state->obj_texts.end())
						m_emit_message(make_error_ptr("Unexpected OLE object reference."));
					else
					{
						std::string obj_text = *m_curr_state->obj_texts_iter;
						std::replace(obj_text.begin(), obj_text.end(), '\x0b', '\n');
						m_emit_message(document::text{.text = obj_text});
						m_curr_state->obj_texts_iter++;
					}
					break;
				}
				default:
					m_curr_state->field_params = "";
					m_curr_state->field_value = "";
			}
		}

		void fieldSeparator(const FLD* fld,
			SharedPtr<const Word97::CHP> chp)
		{
			log_scope();
			m_curr_state->field_part = FIELD_PART_VALUE;
		}

		void fieldEnd(const FLD* fld,
			SharedPtr<const Word97::CHP> chp)
		{
			log_scope();
			UString params = m_curr_state->field_params;
			int i = 0;
			while (i < params.length() && params[i] == ' ') i++;
			UString field_type;
			while (i < params.length() && params[i] != ' ')
			{
				field_type += UString(params[i]);
				i++;
			}
			while (i < params.length() && params[i] == ' ') i++;
			params = params.substr(i);
			UString res_text;
			switch (m_curr_state->field_type)
			{
				case FLT_FILLIN:
					res_text = params + UString(" ") + m_curr_state->field_value;
					break;
				case FLT_EMBED:
					break;
				case FLT_HYPERLINK:
					if (params[0] == '"')
					{
						UString hyperlink_url;
						for (i = 1; i < params.length() && params[i] != '"'; i++)
							hyperlink_url += UString(params[i]);
						m_emit_message(document::link{.url = ustring_to_string(hyperlink_url)});
						m_emit_message(document::text{.text = ustring_to_string(m_curr_state->field_value)});
						m_emit_message(document::close_link{});
					}
					else
						res_text = params + UString(" ") + m_curr_state->field_value;
					break;
				default:
					res_text = m_curr_state->field_value;
			}
			m_curr_state->field_type = FLT_NONE;
			m_curr_state->field_part = FIELD_PART_NONE;
			std::string t = ustring_to_string(res_text);
			std::replace(t.begin(), t.end(), '\x0b', '\n');
			m_emit_message(document::text{.text = t});
		}

		void endOfDocument()
		{
			log_scope();
			if (m_comments_parsed)
				for (int i = 0; i < m_comments.size(); i++)
					if (m_comments[i].fc >= m_prev_par_fc)
					{
						std::string comment_text = m_comments[i].text;
						std::replace(comment_text.begin(), comment_text.end(), '\x0b', '\n');
						m_emit_message(document::comment{.author = m_comments[i].author, .comment = comment_text});
					}
		}
};

class table_handler : public wvWare::TableHandler
{
	private:
		const message_callbacks& m_emit_message;
		current_state& m_current_state;

	public:
		table_handler(const message_callbacks& emit_message, current_state& current_state)
			: m_emit_message(emit_message), m_current_state(current_state)
		{
		}

		void tableRowStart(SharedPtr<const Word97::TAP> tap)
		{
			log_scope();
			if (m_current_state.table_state.empty() || m_current_state.table_state.top() == table_state::in_cell)
			{
				m_emit_message(document::table{});
				m_current_state.table_state.push(table_state::in_table);
			}
			throw_if (m_current_state.table_state.top() != table_state::in_table, errors::uninterpretable_data{});
			m_emit_message(document::table_row{});
			m_current_state.table_state.push(table_state::in_row);
		}

		void tableRowEnd()
		{
			log_scope();
			throw_if (m_current_state.table_state.empty(), errors::uninterpretable_data{});
			if (m_current_state.table_state.top() == table_state::in_cell)
			{
				m_emit_message(document::close_table_cell{});
				m_current_state.table_state.pop();
			}
			throw_if (m_current_state.table_state.empty() || m_current_state.table_state.top() != table_state::in_row, errors::uninterpretable_data{});
			m_emit_message(document::close_table_row{});
			m_current_state.table_state.pop();
		}

		void tableCellStart()
		{
			log_scope();
			throw_if (m_current_state.table_state.empty() || m_current_state.table_state.top() != table_state::in_row, errors::uninterpretable_data{});
			m_emit_message(document::table_cell{});
			m_current_state.table_state.push(table_state::in_cell);
		}

		void tableCellEnd()
		{
			log_scope();
			if (!m_current_state.table_state.empty() && m_current_state.table_state.top() == table_state::in_table)
			{
				m_emit_message(document::close_table{});
				m_current_state.table_state.pop();
			}
			throw_if (m_current_state.table_state.empty() || m_current_state.table_state.top() != table_state::in_cell, errors::uninterpretable_data{});
			m_emit_message(document::close_table_cell{});
			m_current_state.table_state.pop();
		}
};

class subdocument_handler : public wvWare::SubDocumentHandler
{
	private:
		const message_callbacks& m_emit_message;
		current_header_footer* m_curr_header_footer;
	
	public:
		subdocument_handler(const message_callbacks& emit_message, current_header_footer* curr_header_footer)
			: m_emit_message(emit_message), m_curr_header_footer(curr_header_footer)
		{
		}

		virtual void headerStart(HeaderData::Type type)
		{
			log_scope();
			switch (type)
			{
				case HeaderData::HeaderOdd:
				case HeaderData::HeaderEven:
				case HeaderData::HeaderFirst:
					m_curr_header_footer->in_header = true;
					m_emit_message(document::header{});
					break;
				case HeaderData::FooterOdd:
				case HeaderData::FooterEven:
				case HeaderData::FooterFirst:
					m_curr_header_footer->in_footer = true;
					m_emit_message(document::footer{});
					break;
			}
		}

		virtual void headerEnd()
		{
			log_scope();
			if (m_curr_header_footer->in_header)
				m_emit_message(document::close_header{});
			if (m_curr_header_footer->in_footer)
				m_emit_message(document::close_footer{});
			m_curr_header_footer->in_header = false;
			m_curr_header_footer->in_footer = false;
		}
};

doc_parser::doc_parser() = default;

void pimpl_impl<doc_parser>::parse(const data_source& data, const message_callbacks& emit_message)
{
	log_scope(data);

	current_state curr_state;
	auto storage = std::make_unique<thread_safe_ole_storage>(data.span());
	throw_if (!storage->isValid(), storage->getLastError(), errors::uninterpretable_data{});
	emit_message(document::document
		{
			.metadata = [this, &storage, emit_message]()
			{
				attributes::metadata meta;
				parse_oshared_summary_info(*storage, meta, [emit_message](std::exception_ptr e) { emit_message(std::move(e)); }); // ?
				return meta;
			}
		});
	if (storage->enterDirectory("ObjectPool"))
	{
		log_scope();
		std::vector<std::string> obj_list;
		throw_if (!storage->getStreamsAndStoragesList(obj_list), storage->getLastError(), errors::uninterpretable_data{});
		for (size_t i = 0; i < obj_list.size(); ++i)
		{
			log_scope(i, obj_list[i]);
			std::string obj_text;
			std::string current_dir = obj_list[i];
			if (storage->enterDirectory(obj_list[i]))
			{
				std::vector<std::string> obj_list;
				throw_if (!storage->getStreamsAndStoragesList(obj_list), storage->getLastError(), current_dir, errors::uninterpretable_data{});
				if (find(obj_list.begin(), obj_list.end(), "Workbook") != obj_list.end())
				{
					log_scope();
					try
					{
						xls_parser xls{};
						obj_text = xls.parse(*storage, emit_message);
					}
					catch (const std::exception&)
					{
						emit_message(errors::make_nested_ptr(std::current_exception(), make_error("Error while parsing embedded MS Excel workbook.")));
					}
				}
				storage->leaveDirectory();
			}
			curr_state.obj_texts.push_back(obj_text);
		}
	}
	storage->leaveDirectory();
	curr_state.obj_texts_iter = curr_state.obj_texts.begin();
	log::cerr_redirection cerr_redirection;
	SharedPtr<wvWare::Parser> parser;
	{
		std::lock_guard<std::mutex> parser_factory_mutex_2_lock(parser_factory_mutex_2);
		parser = ParserFactory::createParser(storage.release()); //storage will be deleted inside parser from wv2 library
	}
	cerr_redirection.restore();
	throw_if (!parser, "Error while creating parser");
	throw_if (!parser->isOk() && parser->fib().fEncrypted, errors::file_encrypted{});
	throw_if (!parser->isOk(), "Error while creating parser", errors::uninterpretable_data{});
	text_handler text_handler(emit_message, parser, &curr_state);
	parser->setTextHandler(&text_handler);
	table_handler table_handler(emit_message, curr_state);
	parser->setTableHandler(&table_handler);
	subdocument_handler subdocument_handler(emit_message, &curr_state.header_footer);
	parser->setSubDocumentHandler(&subdocument_handler);
	cerr_redirection.redirect();
	bool res = parser->parse();
	cerr_redirection.restore();
	throw_if (!res, "parse() failed", errors::uninterpretable_data{});
	text_handler.endOfDocument();
	emit_message(document::close_document{});
}

continuation doc_parser::operator()(message_ptr msg, const message_callbacks& emit_message)
{
    if (!msg->is<data_source>())
        return emit_message(std::move(msg));

    auto& data = msg->get<data_source>();
    data.assert_not_encrypted();

    if (!data.has_highest_confidence_mime_type_in(supported_mime_types))
        return emit_message(std::move(msg));

    try
    {
        impl().parse(data, emit_message);
    }
    catch (const std::exception& e)
    {
        std::throw_with_nested(make_error("DOC parsing failed"));
    }
    return continuation::proceed;
}

} // namespace docwire
