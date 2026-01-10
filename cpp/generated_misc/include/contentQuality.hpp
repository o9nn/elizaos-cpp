#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-GAMIFICATION_SRC_UTILS_CONTENTQUALITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-GAMIFICATION_SRC_UTILS_CONTENTQUALITY_H
#include "core.h"
#include "@elizaos/core.h"

class ContentQualityResult;

class ContentQualityResult : public object, public std::enable_shared_from_this<ContentQualityResult> {
public:
    using std::enable_shared_from_this<ContentQualityResult>::shared_from_this;
    boolean isValid;

    string reason;

    double score;
};

extern object CONTENT_QUALITY_CONFIG;
extern array<std::shared_ptr<RegExp>> KEYBOARD_MASH_PATTERNS;
extern std::shared_ptr<RegExp> CONSECUTIVE_CHAR_REGEX;
extern std::shared_ptr<RegExp> EMOJI_REGEX;
extern std::shared_ptr<RegExp> URL_REGEX;
extern std::shared_ptr<RegExp> SPECIAL_CHAR_REGEX;
extern array<std::shared_ptr<RegExp>> SPAM_PHRASES;
double calculateEntropy(string text);

object checkRepeatedChars(string text);

object checkRepeatedWords(string text);

object checkKeyboardMash(string text);

object checkEmojiSpam(string text);

object checkUrlSpam(string text);

object checkSpecialCharSpam(string text);

boolean checkSpamPhrases(string text);

std::shared_ptr<ContentQualityResult> checkContentQuality(string text);

boolean quickSpamCheck(string text);

#endif
