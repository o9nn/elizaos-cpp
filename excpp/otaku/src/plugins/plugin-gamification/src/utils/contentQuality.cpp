#include "contentQuality.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double calculateEntropy(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto len = text.length;
    if (len == 0) return 0;

    const std::unordered_map<std::string, double> freq = {};
    for (const auto& char : text.toLowerCase()
        freq[char] = (freq[char] || 0) + 1;
    }

    auto entropy = 0;
    for (const int char in freq) {
        const auto p = freq[char] / len;
        entropy -= p * Math.log2(p);
    }

    return entropy;

}

void checkRepeatedChars(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; ratio: number
}

void checkRepeatedWords(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; uniqueRatio: number
}

void checkKeyboardMash(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; matchRatio: number
}

void checkEmojiSpam(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; ratio: number
}

void checkUrlSpam(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; ratio: number
}

void checkSpecialCharSpam(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    valid: boolean; ratio: number
}

bool checkSpamPhrases(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto trimmed = text.trim();
    for (const auto& pattern : SPAM_PHRASES)
        if (pattern.test(trimmed)) {
            return false; // Is spam;
        }
    }
    return true; // Not spam;

}

ContentQualityResult checkContentQuality(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text || typeof text != 'string') {
        return { isValid: false, reason: 'Empty or invalid content', score: 0 }
    }

    const auto trimmedText = text.trim();

    // Skip very short messages (handled by MIN_CHAT_LENGTH in caller)
    if (trimmedText.length < 10) {
        return { isValid: false, reason: 'Too short', score: 0 }
    }

    auto score = 100;
    const std::vector<std::string> reasons = [];

    // 1. Check repeated characters
    const auto charCheck = checkRepeatedChars(trimmedText);
    if (!charCheck.valid) {
        score -= 40;
        reasons.push('repeated_chars');
        } else {
            score -= charCheck.ratio * 20;
        }

        // 2. Check repeated words
        const auto wordCheck = checkRepeatedWords(trimmedText);
        if (!wordCheck.valid) {
            score -= 35;
            reasons.push('repeated_words');
            } else {
                score -= (1 - wordCheck.uniqueRatio) * 15;
            }

            // 3. Check keyboard mash
            const auto mashCheck = checkKeyboardMash(trimmedText);
            if (!mashCheck.valid) {
                score -= 50;
                reasons.push('keyboard_mash');
                } else {
                    score -= mashCheck.matchRatio * 25;
                }

                // 4. Check entropy
                const auto entropy = calculateEntropy(trimmedText);
                if (entropy < CONTENT_QUALITY_CONFIG.MIN_SHANNON_ENTROPY) {
                    score -= 30;
                    reasons.push('low_entropy');
                }

                // 5. Check emoji spam
                const auto emojiCheck = checkEmojiSpam(trimmedText);
                if (!emojiCheck.valid) {
                    score -= 25;
                    reasons.push('emoji_spam');
                }

                // 6. Check URL spam
                const auto urlCheck = checkUrlSpam(trimmedText);
                if (!urlCheck.valid) {
                    score -= 30;
                    reasons.push('url_spam');
                }

                // 7. Check special char spam
                const auto specialCheck = checkSpecialCharSpam(trimmedText);
                if (!specialCheck.valid) {
                    score -= 25;
                    reasons.push('special_char_spam');
                }

                // 8. Check common spam phrases
                if (!checkSpamPhrases(trimmedText)) {
                    score -= 35;
                    reasons.push('spam_phrase');
                }

                // Normalize score
                score = Math.max(0, Math.min(100, score));

                // Threshold for valid content
                const auto isValid = score >= 50;

                if (!isValid) {
                    logger.debug(
                    { text: trimmedText.substring(0, 50), score, reasons },
                    '[ContentQuality] Message failed quality check';
                    );
                }

                return {
                    isValid,
                    reason: reasons.length > 0 ? reasons.join(', ') : std::nullopt,
                    score,
                    };

}

bool quickSpamCheck(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text || typeof text != 'string') return false;

    const auto trimmed = text.trim();

    // Check consecutive repeats (e.g., "aaaaaaa")
    if (CONSECUTIVE_CHAR_REGEX.test(trimmed)) return false;

    // Check keyboard mash (quick version)
    for (const auto& pattern : KEYBOARD_MASH_PATTERNS.slice(0, 5)
        if (pattern.test(trimmed)) return false;
    }

    // Check spam phrases
    if (!checkSpamPhrases(trimmed)) return false;

    return true;

}

} // namespace elizaos
