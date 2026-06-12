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

TEST(txt_parser, lines)
{
    std::vector<message_ptr> msgs;
    std::string test_input {"Line ends with LF\nLine ends with CR\rLine ends with CRLF\r\nLine without EOL"};
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        txt_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with LF"))),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with CR"))),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with CRLF"))),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line without EOL"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::close_document>(_)
    ));
    
    msgs.clear();
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        txt_parser{parse_paragraphs{true}, parse_lines{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with LF"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("\n"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with CR"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("\r"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line ends with CRLF"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("\r\n"))),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line without EOL"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::close_document>(_)
    ));
    
    msgs.clear();
    docwire::data_source{test_input, mime_type{"text/plain"}, confidence::highest} |
        txt_parser{parse_paragraphs{false}, parse_lines{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq(test_input))),
        MessagePtrWith<document::close_document>(_)
    ));
}

TEST(txt_parser, paragraphs)
{
    std::vector<message_ptr> msgs;
    docwire::data_source{
            std::string{"Paragraph 1 Line 1\nParagraph 1 Line 2\n\nParagraph 2 Line 1"},
            mime_type{"text/plain"}, confidence::highest} |
        txt_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph 1 Line 1"))),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph 1 Line 2"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Paragraph 2 Line 1"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::close_document>(_)
    ));
    
    msgs.clear();
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
        txt_parser{} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::paragraph>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line"))),
        MessagePtrWith<document::close_paragraph>(_),
        MessagePtrWith<document::close_document>(_)
    ));
    
    msgs.clear();
    docwire::data_source{std::string{"\nLine\n"}, mime_type{"text/plain"}, confidence::highest} |
        txt_parser{parse_paragraphs{false}} | msgs;
    ASSERT_THAT(msgs, ElementsAre(
        MessagePtrWith<document::document>(_),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::text>(Field(&document::text::text, StrEq("Line"))),
        MessagePtrWith<document::break_line>(_),
        MessagePtrWith<document::close_document>(_)
    ));    
}
