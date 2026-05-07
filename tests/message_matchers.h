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

#ifndef DOCWIRE_TESTS_MESSAGE_MATCHERS_H
#define DOCWIRE_TESTS_MESSAGE_MATCHERS_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "message.h"
#include "data_source.h"
#include "serialization_document_elements.h" // IWYU pragma: keep
#include "serialization_enum.h" // IWYU pragma: keep
#include "stringification.h"

template <typename T>
class MessagePtrWithMatcher {
public:
    explicit MessagePtrWithMatcher(const testing::Matcher<const T&>& sub_matcher)
        : sub_matcher_(sub_matcher) {}

    void DescribeTo(std::ostream* os) const {
        *os << "is a message_ptr with a value of type " << typeid(T).name() << " that ";
        sub_matcher_.DescribeTo(os);
    }

    void DescribeNegationTo(std::ostream* os) const {
        *os << "is not a message_ptr with a value of type " << typeid(T).name() << " that ";
        sub_matcher_.DescribeNegationTo(os);
    }

    bool MatchAndExplain(const docwire::message_ptr& msg, testing::MatchResultListener* listener) const {
        if (!msg) {
            *listener << "which is a null pointer";
            return false;
        }
        if (!msg->is<T>()) {
            *listener << "which holds a value of type " << msg->object_type().name();
            return false;
        }
        const T& value = msg->get<T>();
        return sub_matcher_.MatchAndExplain(value, listener);
    }

private:
    const testing::Matcher<const T&> sub_matcher_;
};

template <typename T>
testing::PolymorphicMatcher<MessagePtrWithMatcher<T>> MessagePtrWith(const testing::Matcher<const T&>& sub_matcher) {
    return testing::MakePolymorphicMatcher(MessagePtrWithMatcher<T>(sub_matcher));
}

namespace docwire::document {
    inline void PrintTo(const Text& text, std::ostream* os) {
        *os << testing::PrintToString(text.text);
    }
}

namespace docwire {
    inline void PrintTo(const mime_type& mt, std::ostream* os) {
        *os << stringify(mt);
    }

    inline void PrintTo(const confidence& c, std::ostream* os) {
        *os << stringify(c);
    }
}

#endif // DOCWIRE_TESTS_MESSAGE_MATCHERS_H
