#ifndef DOCWIRE_PLAIN_TEXT_WRAP_H
#define DOCWIRE_PLAIN_TEXT_WRAP_H

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace docwire::plain_text {

/**
 * @brief Wraps a single line of text to fit within a given width.
 *
 * Words are broken at spaces. If a single word exceeds the width and
 * @p break_long_words is true, the word is split into chunks of exactly
 * @p width characters. Otherwise, long words may overflow.
 */
inline std::vector<std::string> wrap(const std::string& line, int width, bool break_long_words = true)
{
    if (width <= 0)
        throw std::invalid_argument("width must be positive");

    std::vector<std::string> result;
    if (line.empty()) {
        result.push_back("");
        return result;
    }

    std::istringstream iss(line);
    std::string word, cur;
    while (iss >> word) {
        if (break_long_words) {
            while (word.size() > static_cast<size_t>(width)) {
                if (!cur.empty()) {
                    result.push_back(cur);
                    cur.clear();
                }
                result.push_back(word.substr(0, width));
                word = word.substr(width);
            }
        }
        if (!cur.empty() && cur.size() + 1 + word.size() > static_cast<size_t>(width)) {
            result.push_back(cur);
            cur.clear();
        }
        if (!cur.empty()) cur += ' ';
        cur += word;
    }
    if (!cur.empty())
        result.push_back(cur);
    if (result.empty())
        result.push_back("");
    return result;
}

} // namespace docwire::plain_text

#endif // DOCWIRE_PLAIN_TEXT_WRAP_H
