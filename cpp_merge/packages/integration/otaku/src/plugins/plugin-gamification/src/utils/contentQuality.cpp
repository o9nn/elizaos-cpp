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
    auto dominantRatio = maxCharCount / text->replace((new RegExp(std::string("\s"))), string_empty)->get_length();
    return object{
        object::pair{std::string("valid"), AND((ratio < CONTENT_QUALITY_CONFIG["MAX_CHAR_REPETITION_RATIO"]), (dominantRatio < CONTENT_QUALITY_CONFIG["MAX_CHAR_REPETITION_RATIO"]))}, 
        object::pair{std::string("ratio"), Math->max(ratio, dominantRatio)}
    };
};


object checkRepeatedWords(string text)
{
    auto words = OR((text->toLowerCase()->match((new RegExp(std::string("\b[a-z]+\b/"))))), (array<any>()));
    if (words->length < 3) return object{
        object::pair{std::string("valid"), true}, 
        object::pair{std::string("uniqueRatio"), 1}
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
        object::pair{std::string("valid"), AND((uniqueRatio >= CONTENT_QUALITY_CONFIG["MIN_UNIQUE_WORD_RATIO"]), (repetitionRatio < CONTENT_QUALITY_CONFIG["MAX_WORD_REPETITION_RATIO"]))}, 
        object::pair{std::string("uniqueRatio"), std::string("uniqueRatio")}
    };
};


object checkKeyboardMash(string text)
{
    auto matchedLength = 0;
    auto cleanText = text->replace((new RegExp(std::string("\s"))), string_empty)->toLowerCase();
    for (auto& pattern : KEYBOARD_MASH_PATTERNS)
    {
        auto matches = OR((cleanText->match(pattern)), (array<any>()));
        matchedLength += matches->reduce([=](auto sum, auto m) mutable
        {
            return sum + m->get_length();
        }
        , 0);
    }
    auto ratio = (cleanText->get_length() > 0) ? any(matchedLength / cleanText->get_length()) : any(0);
    return object{
        object::pair{std::string("valid"), ratio < CONTENT_QUALITY_CONFIG["KEYBOARD_MASH_THRESHOLD"]}, 
        object::pair{std::string("matchRatio"), ratio}
    };
};


object checkEmojiSpam(string text)
{
    auto emojis = OR((text->match(EMOJI_REGEX)), (array<any>()));
    auto emojiLength = emojis->length * 2;
    auto ratio = emojiLength / Math->max(text->get_length(), 1);
    return object{
        object::pair{std::string("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_EMOJI_RATIO"]}, 
        object::pair{std::string("ratio"), std::string("ratio")}
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
        object::pair{std::string("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_URL_RATIO"]}, 
        object::pair{std::string("ratio"), std::string("ratio")}
    };
};


object checkSpecialCharSpam(string text)
{
    auto specialChars = OR((text->match(SPECIAL_CHAR_REGEX)), (array<any>()));
    auto ratio = specialChars->length / Math->max(text->get_length(), 1);
    return object{
        object::pair{std::string("valid"), ratio <= CONTENT_QUALITY_CONFIG["MAX_SPECIAL_CHAR_RATIO"]}, 
        object::pair{std::string("ratio"), std::string("ratio")}
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
    if (OR((!text), (type_of(text) != std::string("string")))) {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("reason"), std::string("Empty or invalid content")}, 
            object::pair{std::string("score"), 0}
        };
    }
    auto trimmedText = text->trim();
    if (trimmedText->get_length() < 10) {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("reason"), std::string("Too short")}, 
            object::pair{std::string("score"), 0}
        };
    }
    auto score = 100;
    auto reasons = array<string>();
    auto charCheck = checkRepeatedChars(trimmedText);
    if (!charCheck["valid"]) {
        score -= 40;
        reasons->push(std::string("repeated_chars"));
    } else {
        score -= charCheck["ratio"] * 20;
    }
    auto wordCheck = checkRepeatedWords(trimmedText);
    if (!wordCheck["valid"]) {
        score -= 35;
        reasons->push(std::string("repeated_words"));
    } else {
        score -= (1 - wordCheck["uniqueRatio"]) * 15;
    }
    auto mashCheck = checkKeyboardMash(trimmedText);
    if (!mashCheck["valid"]) {
        score -= 50;
        reasons->push(std::string("keyboard_mash"));
    } else {
        score -= mashCheck["matchRatio"] * 25;
    }
    auto entropy = calculateEntropy(trimmedText);
    if (entropy < CONTENT_QUALITY_CONFIG["MIN_SHANNON_ENTROPY"]) {
        score -= 30;
        reasons->push(std::string("low_entropy"));
    }
    auto emojiCheck = checkEmojiSpam(trimmedText);
    if (!emojiCheck["valid"]) {
        score -= 25;
        reasons->push(std::string("emoji_spam"));
    }
    auto urlCheck = checkUrlSpam(trimmedText);
    if (!urlCheck["valid"]) {
        score -= 30;
        reasons->push(std::string("url_spam"));
    }
    auto specialCheck = checkSpecialCharSpam(trimmedText);
    if (!specialCheck["valid"]) {
        score -= 25;
        reasons->push(std::string("special_char_spam"));
    }
    if (!checkSpamPhrases(trimmedText)) {
        score -= 35;
        reasons->push(std::string("spam_phrase"));
    }
    score = Math->max(0, Math->min(100, score));
    auto isValid = score >= 50;
    if (!isValid) {
        logger->debug(object{
            object::pair{std::string("text"), trimmedText->substring(0, 50)}, 
            object::pair{std::string("score"), std::string("score")}, 
            object::pair{std::string("reasons"), std::string("reasons")}
        }, std::string("[ContentQuality] Message failed quality check"));
    }
    return object{
        object::pair{std::string("isValid"), std::string("isValid")}, 
        object::pair{std::string("reason"), (reasons->get_length() > 0) ? any(reasons->join(std::string(", "))) : any(undefined)}, 
        object::pair{std::string("score"), std::string("score")}
    };
};


boolean quickSpamCheck(string text)
{
    if (OR((!text), (type_of(text) != std::string("string")))) return false;
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
    object::pair{std::string("MAX_CONSECUTIVE_SAME_CHAR"), 4}, 
    object::pair{std::string("MAX_CHAR_REPETITION_RATIO"), 0.5}, 
    object::pair{std::string("MAX_WORD_REPETITION_RATIO"), 0.6}, 
    object::pair{std::string("MIN_UNIQUE_WORD_RATIO"), 0.3}, 
    object::pair{std::string("MIN_SHANNON_ENTROPY"), 2}, 
    object::pair{std::string("MAX_EMOJI_RATIO"), 0.5}, 
    object::pair{std::string("MAX_URL_RATIO"), 0.7}, 
    object::pair{std::string("KEYBOARD_MASH_THRESHOLD"), 0.4}, 
    object::pair{std::string("MAX_SPECIAL_CHAR_RATIO"), 0.4}
};
array<std::shared_ptr<RegExp>> KEYBOARD_MASH_PATTERNS = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("qwert/"))), (new RegExp(std::string("asdf/"))), (new RegExp(std::string("zxcv/"))), (new RegExp(std::string("qazwsx/"))), (new RegExp(std::string("yuiop/"))), (new RegExp(std::string("ghjkl/"))), (new RegExp(std::string("bnm/"))), (new RegExp(std::string("edcrfv/"))), (new RegExp(std::string("tgbyhn/"))), (new RegExp(std::string("ujmik/"))), (new RegExp(std::string("[1234567890]{5,}"))), (new RegExp(std::string("abcdef/"))), (new RegExp(std::string("xyz/"))) };
std::shared_ptr<RegExp> CONSECUTIVE_CHAR_REGEX = (new RegExp(std::string("(.)\1{4,}")));
std::shared_ptr<RegExp> EMOJI_REGEX = (new RegExp(std::string("[\u{1F300}-\u{1F9FF}]|[\u{2600}-\u{26FF}]|[\u{2700}-\u{27BF}]|[\u{1F600}-\u{1F64F}]|[\u{1F680}-\u{1F6FF}]|[\u{1F1E0}-\u{1F1FF}]/")));
std::shared_ptr<RegExp> URL_REGEX = (new RegExp(std::string("https?:\/\/[^\s]+|www\.[^\s]+/")));
std::shared_ptr<RegExp> SPECIAL_CHAR_REGEX = (new RegExp(std::string("[^\w\s.,!?'"()-]")));
array<std::shared_ptr<RegExp>> SPAM_PHRASES = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("^(hey|hi|hello|sup|yo)+$"))), (new RegExp(std::string("^(ok|okay|k|kk|kkk)+$"))), (new RegExp(std::string("^(yes|yeah|yep|yup|no|nope)+$"))), (new RegExp(std::string("^(lol|lmao|rofl|haha|hehe)+$"))), (new RegExp(std::string("^(nice|cool|great|good|bad|wow)+$"))), (new RegExp(std::string("^(gm|gn|gg|ty|thx|thanks)+$"))), (new RegExp(std::string("^[\s\S]*\btest\b[\s\S]*$"))), (new RegExp(std::string("^(please|pls|plz)\s+(help|send|give)"))) };

void Main(void)
{
}

MAIN
