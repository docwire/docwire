#ifndef DOCWIRE_PLAIN_TEXT_OUTPUT_WIDTH_H
#define DOCWIRE_PLAIN_TEXT_OUTPUT_WIDTH_H

#include "../ranged.h"

namespace docwire::plain_text {

/**
 * @brief Strong type representing maximum output line width for plain text.
 *
 * Values are enforced to be at least 20.
 */
class output_width {
public:
    explicit output_width(int w) : m_value(w) {}
    int value() const { return static_cast<int>(m_value); }
private:
    at_least<20, int> m_value;
};

} // namespace docwire::plain_text

#endif
