#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>

class TextProcessor {
public:
    TextProcessor() = default;

    // 1. Replace words
    std::string replace_words(
        const std::string& text, const std::string& old_word, const std::string& new_word) const;

    // 2. Find phrase
    std::string find_phrase(const std::string& text, const std::string& phrase) const;

    // 3. Capitalize each word
    std::string capitalize_words(const std::string& text) const;

    // 4. Capitalize first letter of each sentence
    std::string capitalize_sentences(const std::string& text) const;

    // 5. Number sentences sequentially
    std::string number_sentences(const std::string& text) const;
};

#endif // TEXT_PROCESSOR_H

