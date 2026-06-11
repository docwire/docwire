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

#include "message_matchers.h"
#include "txt_parser.h"
#include "input.h"
#include "output.h"

using namespace docwire;
using namespace testing;

TEST(TXTParser, lines)
{
    std::vector<message_ptr> msgs;
    std::string test_input {"Line ends with LF\nLine ends with CR\rLine ends with CRLF\r\nLine without EOL"};
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with LF"))),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with CR"))),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with CRLF"))),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line without EOL"))),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::CloseDocument>(_)
    ));
    
    msgs.clear();
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{true}, parse_lines{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with LF"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("\n"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with CR"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("\r"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line ends with CRLF"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("\r\n"))),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line without EOL"))),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::CloseDocument>(_)
    ));
    
    msgs.clear();
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{false}, parse_lines{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq(test_input))),
        MessagePtrWith<document::CloseDocument>(_)
    ));
}

TEST(TXTParser, paragraphs)
{
    std::vector<message_ptr> msgs;
    docwire::data_source{
            std::string{"Paragraph 1 Line 1\nParagraph 1 Line 2\n\nParagraph 2 Line 1"},
            mime_type{"text/plain"}, confidence::highest} |
        TXTParser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Paragraph 1 Line 1"))),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Paragraph 1 Line 2"))),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Paragraph 2 Line 1"))),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::CloseDocument>(_)
    ));
    
    msgs.clear();
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::Paragraph>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line"))),
        MessagePtrWith<document::CloseParagraph>(_),
        MessagePtrWith<document::CloseDocument>(_)
    ));
    
    msgs.clear();
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
        TXTParser{parse_paragraphs{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::Document>(_),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::Text>(Field(&document::Text::text, StrEq("Line"))),
        MessagePtrWith<document::BreakLine>(_),
        MessagePtrWith<document::CloseDocument>(_)
    ));    
}
