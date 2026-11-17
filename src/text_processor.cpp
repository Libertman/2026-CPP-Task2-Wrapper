#include "text_processor.h"
#include <cctype>
#include <vector>

namespace {

bool is_utf8_continuation(unsigned char c) noexcept {
    return (c & 0xC0) == 0x80;
}

std::size_t utf8_char_len(unsigned char c) noexcept {
    if (c < 128) {
        return 1;
    }
    if ((c & 0xE0) == 0xC0) {
        return 2;
    }
    if ((c & 0xF0) == 0xE0) {
        return 3;
    }
    if ((c & 0xF8) == 0xF0) {
        return 4;
    }
    return 1;
}

bool is_letter(const std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return false;
    }
    auto c = static_cast<unsigned char>(str[pos]);
    if (c < 128) {
        return std::isalpha(c) != 0;
    }
    // Cyrillic leading bytes in UTF-8
    return (c == 0xD0 || c == 0xD1) && (pos + 1 < str.size());
}

bool is_word_char(const std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return false;
    }
    auto c = static_cast<unsigned char>(str[pos]);
    if (c < 128) {
        return (std::isalnum(c) != 0) || (c == '_');
    }
    return (c == 0xD0 || c == 0xD1) && (pos + 1 < str.size());
}

std::size_t to_upper_letter(std::string& str, std::size_t pos) noexcept {
    if (pos >= str.size()) {
        return 0;
    }
    auto c1 = static_cast<unsigned char>(str[pos]);
    if (c1 < 128) {
        str[pos] = static_cast<char>(std::toupper(c1));
        return 1;
    }
    if (pos + 1 < str.size()) {
        auto c2 = static_cast<unsigned char>(str[pos + 1]);
        if (c1 == 0xD0 && c2 >= 0xB0 && c2 <= 0xBF) {
            str[pos + 1] = static_cast<char>(c2 - 0x20);
            return 2;
        }
        if (c1 == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) {
            str[pos] = static_cast<char>(0xD0);
            str[pos + 1] = static_cast<char>(c2 + 0x20);
            return 2;
        }
        if (c1 == 0xD1 && c2 == 0x91) { // 'ё' -> 'Ё'
            str[pos] = static_cast<char>(0xD0);
            str[pos + 1] = static_cast<char>(0x81);
            return 2;
        }
        return 2;
    }
    return 1;
}

std::size_t find_char_start_before(const std::string& str, std::size_t pos) noexcept {
    std::size_t prev = pos - 1;
    while (prev > 0 && is_utf8_continuation(static_cast<unsigned char>(str[prev]))) {
        --prev;
    }
    return prev;
}

bool check_boundary_before(
    const std::string& text, const std::string& old_word, std::size_t match_pos) noexcept {
    if (match_pos == 0) {
        return true;
    }
    std::size_t prev_char_pos = find_char_start_before(text, match_pos);
    return !(is_word_char(text, prev_char_pos) && is_word_char(old_word, 0));
}

bool check_boundary_after(
    const std::string& text, const std::string& old_word, std::size_t match_pos) noexcept {
    std::size_t next_pos = match_pos + old_word.size();
    if (next_pos >= text.size()) {
        return true;
    }
    std::size_t last_old_char_pos = find_char_start_before(old_word, old_word.size());
    return !(is_word_char(old_word, last_old_char_pos) && is_word_char(text, next_pos));
}

std::string trim(const std::string& s) {
    std::size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    std::size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

std::vector<std::string> extract_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::size_t start = 0;
    std::size_t i = 0;
    while (i < text.size()) {
        char c = text[i];
        if (c == '.' || c == '!' || c == '?') {
            while (i + 1 < text.size()) {
                char next_c = text[i + 1];
                if (next_c != '.' && next_c != '!' && next_c != '?') {
                    break;
                }
                ++i;
            }
            std::string sentence = trim(text.substr(start, (i + 1) - start));
            if (!sentence.empty()) {
                sentences.push_back(sentence);
            }
            start = i + 1;
        }
        ++i;
    }
    if (start < text.size()) {
        std::string sentence = trim(text.substr(start));
        if (!sentence.empty()) {
            sentences.push_back(sentence);
        }
    }
    return sentences;
}

} // namespace

std::string TextProcessor::replace_words(
    const std::string& text, const std::string& old_word, const std::string& new_word) const {
    if (old_word.empty()) {
        return text;
    }
    std::string result;
    std::size_t pos = 0;
    while (pos < text.size()) {
        std::size_t match_pos = text.find(old_word, pos);
        if (match_pos == std::string::npos) {
            result.append(text, pos, text.size() - pos);
            break;
        }
        if (check_boundary_before(text, old_word, match_pos) &&
            check_boundary_after(text, old_word, match_pos)) {
            result.append(text, pos, match_pos - pos);
            result.append(new_word);
            pos = match_pos + old_word.size();
        } else {
            result.append(text, pos, match_pos - pos + 1);
            pos = match_pos + 1;
        }
    }
    return result;
}

std::string TextProcessor::find_phrase(
    const std::string& text, const std::string& phrase) const {
    if (phrase.empty()) {
        return "не найдено";
    }
    if (text.find(phrase) != std::string::npos) {
        return "найдено";
    }
    return "не найдено";
}

std::string TextProcessor::capitalize_words(const std::string& text) const {
    std::string result = text;
    bool in_word = false;
    std::size_t i = 0;
    while (i < result.size()) {
        if (is_letter(result, i)) {
            if (!in_word) {
                i += to_upper_letter(result, i);
                in_word = true;
            } else {
                i += utf8_char_len(static_cast<unsigned char>(result[i]));
            }
        } else {
            in_word = is_word_char(result, i);
            i += utf8_char_len(static_cast<unsigned char>(result[i]));
        }
    }
    return result;
}

std::string TextProcessor::capitalize_sentences(const std::string& text) const {
    std::string result = text;
    bool new_sentence = true;
    std::size_t i = 0;
    while (i < result.size()) {
        char c = result[i];
        if (new_sentence && is_letter(result, i)) {
            i += to_upper_letter(result, i);
            new_sentence = false;
        } else {
            if (c == '.' || c == '!' || c == '?') {
                new_sentence = true;
            }
            i += utf8_char_len(static_cast<unsigned char>(result[i]));
        }
    }
    return result;
}

std::string TextProcessor::number_sentences(const std::string& text) const {
    std::vector<std::string> sentences = extract_sentences(text);
    std::string result;
    for (std::size_t idx = 0; idx < sentences.size(); ++idx) {
        if (idx > 0) {
            result += "\n";
        }
        result += std::to_string(idx + 1) + ". " + sentences[idx];
    }
    return result;
}

