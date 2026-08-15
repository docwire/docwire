#ifndef DOCWIRE_PLAIN_TEXT_WRAP_LINES_H
#define DOCWIRE_PLAIN_TEXT_WRAP_LINES_H

#include "wrap.h"
#include <string>
#include <vector>

namespace docwire::plain_text {

/**
 * @brief Wraps multiple lines to fit within a given width.
 *
 * Each input line is wrapped using wrap(), and the resulting lines are
 * concatenated into a single output vector.
 *
 * @param lines input lines to wrap
 * @param width maximum line length
 * @return flattened vector of wrapped lines
 */
inline std::vector<std::string> wrap_lines(const std::vector<std::string>& lines, int width)
{
    std::vector<std::string> wrapped;
    for (const auto& line : lines) {
        auto w = wrap(line, width);
        wrapped.insert(wrapped.end(), w.begin(), w.end());
    }
    if (wrapped.empty())
        wrapped.push_back("");
    return wrapped;
}

} // namespace docwire::plain_text

#endif
