#ifndef DOCWIRE_MESSAGE_COUNTERS_H
#define DOCWIRE_MESSAGE_COUNTERS_H

#include "message.h"
#include "document_elements.h"
#include <exception>

namespace docwire {

struct message_counters {
    size_t attempts = 0;
    size_t successes = 0;

    bool all_failed() const {
        return attempts > 0 && successes == 0;
    }
};

inline bool is_framing_message(const message_base& msg) {
    return msg.is<document::document>() || msg.is<document::close_document>() ||
           msg.is<document::page>() || msg.is<document::close_page>() ||
           msg.is<document::break_line>() ||
           msg.is<std::exception_ptr>();
}

inline message_callbacks make_counted_message_callbacks(const message_callbacks& original, message_counters& counters) {
    auto wrapper = [&original, &counters](message_ptr msg, bool is_back) -> continuation {
         bool is_framing = is_framing_message(*msg);
         if (!is_framing) counters.attempts++;
         auto c = is_back ? original.back(std::move(msg)) : original.further(std::move(msg));
         if (!is_framing) counters.successes++;
         return c;
    };
    
    return message_callbacks {
        .m_further = [wrapper](message_ptr msg) { return wrapper(std::move(msg), false); },
        .m_back = [wrapper](message_ptr msg) { return wrapper(std::move(msg), true); }
    };
}

}

#endif