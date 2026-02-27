#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/utils/contentQuality.h"

double calculateEntropy(string text)
{
    auto len = text->get_length();
    if (len == 0) return 0;
    auto freq = object{};
    for (auto& char : text->toLowerCase())
    {
        freq[char] = (OR((const_(freq)[char]), (0))) + 1;
    }
    auto entropy = 0;
    for (auto& char : keys_(freq))
    {
        auto p = const_(freq)[char] / len;
        entropy -= p * Math->log2(p);
    }
    return entropy;
};


object checkRepeatedChars(string text)
{
    auto matches = OR((text->match(CONSECUTIVE_CHAR_REGEX)), (array<any>()));
    auto repeatedLength = matches->reduce([=](auto sum, auto m) mutable
    {
        return sum + m->get_length();
    }
    , 0);
    auto ratio = repeatedLength / text->get_length();
    auto charCounts = object{};
    for (auto& char : text->toLowerCase())
    {
        if (char->trim()) {
            charCounts[char] = (OR((const_(charCounts)[char]), (0))) + 1;
        }
    }
    auto maxCharCount = Math->max(const_(Object->values(charCounts))[0], 0);
    auto dominantRatio = maxCharCount / text->replace((new RegExp(std:("\s"))), string_empty)->get_length();
    return object{
        object::pair{std:("valid"), AND((ratio < CONTENT_QUALITY_CONFIG["MAX_CHAR_REPETITION_RATIO"]), (dominantRatio < CONTENT_QUALITY_CONFIG["MAX_CHAR_REPETITION_RATIO"]))}, 
        object::pair{std:("ratio"), Math->max(ratio, dominantRatio)}
    };
};


object checkRepeatedWords(string text)
{
    auto words = OR((text->toLowerCase()->match((new RegExp(std:("\b[a-z]+\b/"))))), (array<any>()));
    if (words->length < 3) return object{
        object::pair{std:("valid"), true}, 
        object::pair{std:("uniqueRatio"), 1}
    };
    auto uniqueWords = std::make_shared<Set>(words);
    auto uniqueRatio = uniqueWords->size / words->length;
    auto wordCounts = object{};
    for (auto& word : words)
    {
        wordCounts[word] = (OR((const_(wordCounts)[word]), (0))) + 1;
    }
    auto maxRepetition = Math->max(const_(Object->values(wordCounts))[0]);
    auto repetitionRatio = maxRepetition / words->length;
    return object{
        object::pair{std:("valid"), AND((uniqueRatio >= CONTENT_QUALITY_CONFIG["MIN_UNIQUE_WORD_RATIO"]), (repetitionRatio < CONTENT_QUALITY_CONFIG["MAX_WORD_REPETITION_RATIO"]))}, 
        object::pair{std:("uniqueRatio"), std:("uniqueRatio")}
    };
};


object checkKeyboardMash(string text)
{
    auto matchedLength = 0;
    auto cleanText = text->replace((new RegExp(std:("\s"))), string_empty)->toLowerCase();
    for (auto& pattern : KEYBOARD_MASH_PATTERNS)
    {
        auto matches = OR((cleanText->match(pattern)), (array<any>()));
        matchedLength += matches->reduce([=](auto sum, auto m) mutable
        {
            return sum + m->get_length();
        }
        , 0);
    }
    auto ratio = (cleanText->get_length() > 0) ? any(matchedLength / cleanText->get_length()) (0);
    return object{
        object::pair{std:("valid"), ratio < CONTENT_QUALITY_CONFIG["KEYBOARD_MASH_THRESHOLD"]}, 
        object::pair{std:("matchRatio"), ratio}
    };
};


object checkEmojiSpam(string text)
{
    auto emojis = OR((text->match(EMOJI_REGEX)), (array<any>()));
    auto emojiLength = emojis->length * 2;
    auto ratio = emojiLength / Math->max(text->get_length(), 1);
    return object{
        object::pair{std:("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_EMOJI_RATIO"]}, 
        object::pair{std:("ratio"), std:("ratio")}
    };
};


object checkUrlSpam(string text)
{
    auto urls = OR((text->match(URL_REGEX)), (array<any>()));
    auto urlLength = urls->reduce([=](auto sum, auto url) mutable
    {
        return sum + url->get_length();
    }
    , 0);
    auto ratio = urlLength / Math->max(text->get_length(), 1);
    return object{
        object::pair{std:("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_URL_RATIO"]}, 
        object::pair{std:("ratio"), std:("ratio")}
    };
};


object checkSpecialCharSpam(string text)
{
    auto specialChars = OR((text->match(SPECIAL_CHAR_REGEX)), (array<any>()));
    auto ratio = specialChars->length / Math->max(text->get_length(), 1);
    return object{
        object::pair{std:("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_SPECIAL_CHAR_RATIO"]}, 
        object::pair{std:("ratio"), std:("ratio")}
    };
};


boolean checkSpamPhrases(string text)
{
    auto trimmed = text->trim();
    for (auto& pattern : SPAM_PHRASES)
    {
        if (pattern->test(trimmed)) {
            return false;
        }
    }
    return true;
};


std::shared_ptr<ContentQualityResult> checkContentQuality(string text)
{
    if (OR((!text), (type_of(text) != std:("string")))) {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("reason"), std:("Empty or invalid content")}, 
            object::pair{std:("score"), 0}
        };
    }
    auto trimmedText = text->trim();
    if (trimmedText->get_length() < 10) {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("reason"), std:("Too short")}, 
            object::pair{std:("score"), 0}
        };
    }
    auto score = 100;
    auto reasons = array<string>();
    auto charCheck = checkRepeatedChars(trimmedText);
    if (!charCheck["valid"]) {
        score -= 40;
        reasons->push(std:("repeated_chars"));
    } else {
        score -= charCheck["ratio"] * 20;
    }
    auto wordCheck = checkRepeatedWords(trimmedText);
    if (!wordCheck["valid"]) {
        score -= 35;
        reasons->push(std:("repeated_words"));
    } else {
        score -= (1 - wordCheck["uniqueRatio"]) * 15;
    }
    auto mashCheck = checkKeyboardMash(trimmedText);
    if (!mashCheck["valid"]) {
        score -= 50;
        reasons->push(std:("keyboard_mash"));
    } else {
        score -= mashCheck["matchRatio"] * 25;
    }
    auto entropy = calculateEntropy(trimmedText);
    if (entropy < CONTENT_QUALITY_CONFIG["MIN_SHANNON_ENTROPY"]) {
        score -= 30;
        reasons->push(std:("low_entropy"));
    }
    auto emojiCheck = checkEmojiSpam(trimmedText);
    if (!emojiCheck["valid"]) {
        score -= 25;
        reasons->push(std:("emoji_spam"));
    }
    auto urlCheck = checkUrlSpam(trimmedText);
    if (!urlCheck["valid"]) {
        score -= 30;
        reasons->push(std:("url_spam"));
    }
    auto specialCheck = checkSpecialCharSpam(trimmedText);
    if (!specialCheck["valid"]) {
        score -= 25;
        reasons->push(std:("special_char_spam"));
    }
    if (!checkSpamPhrases(trimmedText)) {
        score -= 35;
        reasons->push(std:("spam_phrase"));
    }
    score = Math->max(0, Math->min(100, score));
    auto isValid = score >= 50;
    if (!isValid) {
        logger->debug(object{
            object::pair{std:("text"), trimmedText->substring(0, 50)}, 
            object::pair{std:("score"), std:("score")}, 
            object::pair{std:("reasons"), std:("reasons")}
        }, std:("[ContentQuality] Message failed quality check"));
    }
    return object{
        object::pair{std:("isValid"), std:("isValid")}, 
        object::pair{std:("reason"), (reasons->get_length() > 0) ? any(reasons->join(std:(", "))) (undefined)}, 
        object::pair{std:("score"), std:("score")}
    };
};


boolean quickSpamCheck(string text)
{
    if (OR((!text), (type_of(text) != std:("string")))) return false;
    auto trimmed = text->trim();
    if (CONSECUTIVE_CHAR_REGEX->test(trimmed)) return false;
    for (auto& pattern : KEYBOARD_MASH_PATTERNS->slice(0, 5))
    {
        if (pattern->test(trimmed)) return false;
    }
    if (!checkSpamPhrases(trimmed)) return false;
    return true;
};


object CONTENT_QUALITY_CONFIG = object{
    object::pair{std:("MAX_CONSECUTIVE_SAME_CHAR"), 4}, 
    object::pair{std:("MAX_CHAR_REPETITION_RATIO"), 0.5}, 
    object::pair{std:("MAX_WORD_REPETITION_RATIO"), 0.6}, 
    object::pair{std:("MIN_UNIQUE_WORD_RATIO"), 0.3}, 
    object::pair{std:("MIN_SHANNON_ENTROPY"), 2}, 
    object::pair{std:("MAX_EMOJI_RATIO"), 0.5}, 
    object::pair{std:("MAX_URL_RATIO"), 0.7}, 
    object::pair{std:("KEYBOARD_MASH_THRESHOLD"), 0.4}, 
    object::pair{std:("MAX_SPECIAL_CHAR_RATIO"), 0.4}
};
array<std::shared_ptr<RegExp>> KEYBOARD_MASH_PATTERNS = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("qwert/"))), (new RegExp(std:("asdf/"))), (new RegExp(std:("zxcv/"))), (new RegExp(std:("qazwsx/"))), (new RegExp(std:("yuiop/"))), (new RegExp(std:("ghjkl/"))), (new RegExp(std:("bnm/"))), (new RegExp(std:("edcrfv/"))), (new RegExp(std:("tgbyhn/"))), (new RegExp(std:("ujmik/"))), (new RegExp(std:("[1234567890]{5,}"))), (new RegExp(std:("abcdef/"))), (new RegExp(std:("xyz/"))) };
std::shared_ptr<RegExp> CONSECUTIVE_CHAR_REGEX = (new RegExp(std:("(.)\1{4,}")));
std::shared_ptr<RegExp> EMOJI_REGEX = (new RegExp(std:("[\u{1F300}-\u{1F9FF}]|[\u{2600}-\u{26FF}]|[\u{2700}-\u{27BF}]|[\u{1F600}-\u{1F64F}]|[\u{1F680}-\u{1F6FF}]|[\u{1F1E0}-\u{1F1FF}]/")));
std::shared_ptr<RegExp> URL_REGEX = (new RegExp(std:("https?:\/\/[^\s]+|www\.[^\s]+/")));
std::shared_ptr<RegExp> SPECIAL_CHAR_REGEX = (new RegExp(std:("[^\w\s.,!?'"()-]")));
array<std::shared_ptr<RegExp>> SPAM_PHRASES = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("^(hey|hi|hello|sup|yo)+$"))), (new RegExp(std:("^(ok|okay|k|kk|kkk)+$"))), (new RegExp(std:("^(yes|yeah|yep|yup|no|nope)+$"))), (new RegExp(std:("^(lol|lmao|rofl|haha|hehe)+$"))), (new RegExp(std:("^(nice|cool|great|good|bad|wow)+$"))), (new RegExp(std:("^(gm|gn|gg|ty|thx|thanks)+$"))), (new RegExp(std:("^[\s\S]*\btest\b[\s\S]*$"))), (new RegExp(std:("^(please|pls|plz)\s+(help|send|give)"))) };

void Main(void)
{
}

MAIN
