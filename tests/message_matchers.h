/**************************************************************************/
/*                                                                        */
/*  DF-0: Data processing Framework with Zero-overhead                    */
/*                                                                        */
/*  Copyright (c) DF-0 Team, project website https://github.com/df0/df0   */
/*  SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-DF0-Commercial   */
/*                                                                        */
/**************************************************************************/

#ifndef DF0_TESTS_MESSAGE_MATCHERS_H
#define DF0_TESTS_MESSAGE_MATCHERS_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "message.h"
#include "data_source.h"
#include "serialization_document_elements.h" // IWYU pragma: keep
#include "serialization_enum.h" // IWYU pragma: keep
#include "stringification.h"

template <typename T>
class message_ptr_with_matcher {
public:
    explicit message_ptr_with_matcher(const testing::Matcher<const T&>& sub_matcher)
        : sub_matcher_(sub_matcher) {}

    void DescribeTo(std::ostream* os) const {
        *os << "is a message_ptr with a value of type " << typeid(T).name() << " that ";
        sub_matcher_.DescribeTo(os);
    }

    void DescribeNegationTo(std::ostream* os) const {
        *os << "is not a message_ptr with a value of type " << typeid(T).name() << " that ";
        sub_matcher_.DescribeNegationTo(os);
    }

    bool MatchAndExplain(const df0::message_ptr& msg, testing::MatchResultListener* listener) const {
        if (!msg) {
            *listener << "which is a null pointer";
            return false;
        }
        if (!msg->is<T>()) {
            *listener << "which holds a value of type " << msg->object_type_info().name();
            return false;
        }
        const T& value = msg->get<T>();
        return sub_matcher_.MatchAndExplain(value, listener);
    }

private:
    const testing::Matcher<const T&> sub_matcher_;
};

template <typename T>
testing::PolymorphicMatcher<message_ptr_with_matcher<T>> MessagePtrWith(const testing::Matcher<const T&>& sub_matcher) {
    return testing::MakePolymorphicMatcher(message_ptr_with_matcher<T>(sub_matcher));
}

namespace df0::document {
    inline void PrintTo(const text& text, std::ostream* os) {
        *os << testing::PrintToString(text.text);
    }
}

namespace df0 {
    inline void PrintTo(const mime_type& mt, std::ostream* os) {
        *os << stringify(mt);
    }

    inline void PrintTo(const confidence& c, std::ostream* os) {
        *os << stringify(c);
    }
}

#endif // DF0_TESTS_MESSAGE_MATCHERS_H
