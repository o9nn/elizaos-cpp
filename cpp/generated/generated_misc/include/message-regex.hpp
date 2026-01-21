#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_MESSAGE-REGEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_MESSAGE-REGEX_H
#include "core.h"

class MessageRegex;

class MessageRegex : public object, public std::enable_shared_from_this<MessageRegex> {
public:
    using std::enable_shared_from_this<MessageRegex>::shared_from_this;
    static std::shared_ptr<RegExp> BOLD;

    static std::shared_ptr<RegExp> LINK;

    static std::shared_ptr<RegExp> QUOTE;

    static std::shared_ptr<RegExp> CHANNEL_MENTION;

    static std::shared_ptr<RegExp> HYPER_LINK;

    static std::shared_ptr<RegExp> UNDER_LINE;

    static std::shared_ptr<RegExp> ITALICS;

    static std::shared_ptr<RegExp> CODE;

    static std::shared_ptr<RegExp> USER_MENTION;

    static std::shared_ptr<RegExp> EMOJI;

    static std::shared_ptr<RegExp> WINDOWS_INVALID_CHARACTERS;
};

#endif
