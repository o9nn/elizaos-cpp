#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-GAMIFICATION_SRC_UTILS_CONTENTQUALITY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-GAMIFICATION_SRC_UTILS_CONTENTQUALITY_H
#include "core.h"
#include "@elizaos/core.h"

class ContentQualityResult;

class ContentQualityResult : public object, public std::enable_shared_from_this<ContentQualityResult> {
public:
    using std::enable_shared_from_this<ContentQualityResult>::shared_from_this;
    boolean isValid;

    std::string reason;

    double score;
};

extern object CONTENT_QUALITY_CONFIG;
extern array<std::shared_ptr<RegExp>> KEYBOARD_MASH_PATTERNS;
extern std::shared_ptr<RegExp> CONSECUTIVE_CHAR_REGEX;
extern std::shared_ptr<RegExp> EMOJI_REGEX;
extern std::shared_ptr<RegExp> URL_REGEX;
extern std::shared_ptr<RegExp> SPECIAL_CHAR_REGEX;
extern array<std::shared_ptr<RegExp>> SPAM_PHRASES;
double calculateEntropy(std::string text);

object checkRepeatedChars(std::string text);

object checkRepeatedWords(std::string text);

object checkKeyboardMash(std::string text);

object checkEmojiSpam(std::string text);

object checkUrlSpam(std::string text);

object checkSpecialCharSpam(std::string text);

boolean checkSpamPhrases(std::string text);

std::shared_ptr<ContentQualityResult> checkContentQuality(std::string text);

boolean quickSpamCheck(std::string text);

#endif
